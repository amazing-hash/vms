#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <map>
#include <stdexcept> 
#include <fstream>
#include <mutex>
#include <signal.h>
#include <queue>
#include <set>
#include <condition_variable>
#include <sys/resource.h>
#include <tuple>

#include "common/plog/Log.h"
#include "common/plog/Appenders/ColorConsoleAppender.h"

#include "cameracapture/CameraCaptureMJPEG.h"
#include "cameracapture/CameraCaptureMJPEGOverRTSP.h"
#include "cameracapture/CameraCaptureH264OverRTSP.h"
#include "session/Session.h"
#include "frame.h"
#include "session/ISession.h"
#include "session/IStateResponse.h"
#include "session/IStateRequest.h"
#include "session/StateResponseImpl.h"
#include "session/StateRequestImpl.h"

#include "statistics/Statistics.h"

#include "common/json/json.h"
#include <sstream>
#include <boost/lexical_cast/bad_lexical_cast.hpp>

#include "storage/CamerasStorage.h"
#include "videowriter/VideoWriter.h"

#include "routes/routes.h"


#define MAX_EVENTS 64

plog::RollingFileAppender<plog::TxtFormatter> * file_log_appender = new plog::RollingFileAppender<plog::TxtFormatter>("/var/log/vms-duo/log", 10000000, 10);
plog::ColorConsoleAppender<plog::TxtFormatter> * console_log_appender = new plog::ColorConsoleAppender<plog::TxtFormatter>;


std::mutex mutex_configuration_file;

std::map<int, Frame> map_frames;
std::mutex mutex_map_frames;

std::map<int, vms::ISession *> map_sessions;
std::mutex mutex_map_sessions;

std::set<int> unique_indexes;
std::mutex mutex_unique_indexes;

int epool_fd;

using json = nlohmann::json;

void release_session(int socket) {
    auto it = map_sessions.find(socket);
    if (it != map_sessions.end()) {
        delete it->second;
        map_sessions.erase(it);
    }
}

struct rlimit resource_limits;
void setMaxOpenFiles();

int handler_error_opencv(int status, const char* func_name, const char* err_msg, const char* file_name, int line, void* userdata) {
    return 0;
}

int main() {

//    cv::redirectError(handler_error_opencv);

    signal(SIGPIPE, SIG_IGN);
    
    setMaxOpenFiles();

    plog::init(plog::debug, file_log_appender).addAppender(console_log_appender);
    
    vms::Dispatcher::addRoute("^/index.html?", "GET", fetch_index_page);
    vms::Dispatcher::addRoute("^/statistics.html?", "GET", fetch_statistics_page);
    vms::Dispatcher::addRoute("^/camera-view.html.+", "GET", fetch_camera_view_page);
    vms::Dispatcher::addRoute("^/configuration.html?", "GET", fetch_camera_configuration_page);
    
    vms::Dispatcher::addRoute("^/css/.+", "GET", fetch_css_resources);
    vms::Dispatcher::addRoute("^/js/.+", "GET", fetch_js_resources);
    vms::Dispatcher::addRoute("^/svg/.+", "GET", fetch_svg_resources);
    
    vms::Dispatcher::addRoute("^/api/camera/view/channel/\\d+/\\d+", "GET", fetch_camera_stream);
    
    vms::Dispatcher::addRoute("^/api/configuration/cameras/save?", "POST", save_camera_settings);
    
    struct sockaddr_in addr;
    int client_socket;
    int serves_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (serves_socket == -1) {
        LOGE << "Error create server socket. " << strerror(errno);
        exit(EXIT_FAILURE);
    }
    int enable = 1;
    if (setsockopt(serves_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof (int)) < 0) {
        LOGE << "Setsockopt(SO_REUSEADDR) failed";
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8001);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(serves_socket, (struct sockaddr*) &addr, sizeof (addr)) == -1) {
        LOGE << "Error bind server socket. " << strerror(errno);
        exit(EXIT_FAILURE);
    }

    if (listen(serves_socket, 10000) == -1) {
        LOGE << "Error listen server socket. " << strerror(errno);
        exit(EXIT_FAILURE);
    }

    epool_fd = epoll_create1(0);
    if (epool_fd < 0) {
        LOGE << "Error create epoll";
        exit(EXIT_FAILURE);
    }
    // Принудительно создали объект статистики, чтобы не было конкуренции и не ставить mutex
    vms::Statistics::getInstance();

    vms::CameraStorage * camera_storage = vms::CameraStorage::getInstance();
    camera_storage->read_cameras("db/cameras.json");
    json json_cameras = camera_storage->getJSON();
    for (auto json_camera : json_cameras["cameras"]) {
        std::string login = json_camera["login"].get<std::string>();
        std::string password = json_camera["password"].get<std::string>();
        int port = json_camera["port"].get<int>();
        int id = json_camera["id"].get<int>();
        for (auto json_camera_channel : json_camera["channels"]) {
            std::string state = json_camera_channel["state"].get<std::string>();
            if (state == "off")
                continue;
            std::thread([login, password, port, id, json_camera_channel]() {
                while (true) {
                    try {
                        std::string url = json_camera_channel["url"].get<std::string>();
                        std::string i_type = json_camera_channel["i-type"].get<std::string>();
                        std::string o_type = json_camera_channel["o-type"].get<std::string>();
                        std::string i_codec = json_camera_channel["i-codec"].get<std::string>();
                        std::string o_codec = json_camera_channel["o-codec"].get<std::string>();
                        std::string prefix_file_storage = json_camera_channel["storage"]["prefix_file_storage"].get<std::string>();
                        std::string storage_state = json_camera_channel["storage"]["state"].get<std::string>();
                        int count_chunks = json_camera_channel["storage"]["count_chunks"].get<int>();
                        vms::VideoWriter * video_writer = nullptr;
                        int fps = json_camera_channel["fps"].get<int>();
                        int width = json_camera_channel["width"].get<int>();
                        int height = json_camera_channel["height"].get<int>();
                        int channel = json_camera_channel["channel"].get<int>();
                        int unique_index = id * 100 + channel;
                        mutex_unique_indexes.lock();
                        if (unique_indexes.find(unique_index) != unique_indexes.end()) {
                            LOGE << "Violation of uniqueness";
                            mutex_unique_indexes.unlock();
                            exit(EXIT_FAILURE);
                        }
                        mutex_unique_indexes.unlock();

                        vms::cameracapture::ICameraCapture * camera_capture = nullptr;
                        if (i_codec == "mjpeg" && i_type == "tcp/http" && o_codec == "mjpeg" && o_type == "tcp/http") {
                            if (storage_state == "on")
                                video_writer = new vms::VideoWriter(prefix_file_storage, CV_FOURCC('M', 'J', 'P', 'G'), fps, width, height, count_chunks);
                            camera_capture = new vms::cameracapture::CameraCaptureMJPEG(url, login, password, port);
                        } else if (i_codec == "mjpeg" && i_type == "tcp/rtsp" && o_codec == "mjpeg" && o_type == "tcp/http") {
                            if (storage_state == "on")
                                video_writer = new vms::VideoWriter(prefix_file_storage, CV_FOURCC('M', 'J', 'P', 'G'), fps, width, height, count_chunks);
                            camera_capture = new vms::cameracapture::CameraCaptureMJPEGOverRTSP(url, login, password);
                        } else if (i_codec == "h264" && i_type == "tcp/rtsp" && o_codec == "mjpeg" && o_type == "tcp/http") {
                            if (storage_state == "on")
                                video_writer = new vms::VideoWriter(prefix_file_storage, CV_FOURCC('M', 'P', '4', 'V'), fps, width, height, count_chunks);
                            camera_capture = new vms::cameracapture::CameraCaptureH264OverRTSP(url, login, password);
                        } else {
                            LOGE << "Unsupported data";
                            exit(EXIT_FAILURE);
                        }
                        if(json_camera_channel.find("decode_to_mat_opencv") != json_camera_channel.end()){
                            if(json_camera_channel["decode_to_mat_opencv"].get<std::string>() == "no")
                                camera_capture->setDecodeToMatOpenCV(false);
                        }
                            
                        while (true) {
                            std::tuple<std::shared_ptr<std::vector<unsigned char>>, cv::Mat> t = camera_capture->fetchNextFrame();
                            std::shared_ptr<std::vector<unsigned char>> v = std::get<0>(t);
                            cv::Mat frame = std::get<1>(t);
                            if (v == nullptr)
                                continue;
                            if (video_writer && i_codec == "mjpeg") {
                                cv::Mat img = cv::imdecode(*v.get(), CV_LOAD_IMAGE_COLOR);
                                video_writer->write(img);
                            }
                            if (video_writer && i_codec == "h264") {
                                video_writer->write(frame);
                            }
                            mutex_map_frames.lock();
                            map_frames[unique_index].bytes.clear();
                            map_frames[unique_index].bytes.resize(v.get()->size());
                            map_frames[unique_index].image = frame.clone();
                            std::copy(v.get()->begin(), v.get()->end(), map_frames[unique_index].bytes.begin());
                            map_frames[unique_index].id++;
                            mutex_map_frames.unlock();
                        }
                    } catch (vms::cameracapture::exception::CameraCaptureException & ex) {
                        LOGE << ex.what() << "Try to connect";
                        sleep(5);
                    }
                }
            }).detach();
        }
    }

    LOGI << "Server started success";
    std::thread([&] {
        while (true) {
            struct epoll_event * events;
            events = (epoll_event*) malloc(sizeof (struct epoll_event) * MAX_EVENTS);
            if (events) {
                int count_events = epoll_wait(epool_fd, events, MAX_EVENTS, -1);
                if (count_events < 0) {
                    LOGE << "Error epoll_wait";
                    free(events);
                    continue;
                }
                for (ssize_t i = 0; i < count_events; i++) {
                    if (events[i].events & EPOLLIN) {
                        mutex_map_sessions.lock();
                        vms::ISession * session = nullptr;
                        try {
                            session = map_sessions.at(events[i].data.fd);
                            session->m_handler_request(session);
                        } catch (std::out_of_range & ex) {
                            mutex_map_sessions.unlock();
                            LOGE << "Session not found id = " << events[i].data.fd;
                            continue;
                        } catch (vms::session::exception::SessionException & ex) {
                            mutex_map_sessions.unlock();
                            LOGE << ex.what();
                            continue;
                        }
                        /*
                         * Не можем обработать запрос от клиента уничтожаем соединение
                         */
                        if (session->getStateRequset()->id() == vms::STATE_REQUEST_ID::STATE_REQUEST_DESTROY_ID || session->getStateResponse()->id() == vms::STATE_RESPONSE_ID::STATE_RESPONSE_DESTROY_ID) {
                            close(session->m_socket);
                            session->notifyObserversClose();
                            release_session(session->m_socket);

                            mutex_map_sessions.unlock();
                            continue;
                        }
                        /*
                         * Данные полученны от клиента можно начать отсылать данные обратно,
                         * session response переведен в состояние готовности к отдачи
                         */
                        if (session->getStateResponse()->id() == vms::STATE_RESPONSE_ID::STATE_RESPONSE_READY_ID) {
                            struct epoll_event event;
                            event.data.fd = session->m_socket;
                            event.events = EPOLLOUT;
                            if (epoll_ctl(epool_fd, EPOLL_CTL_MOD, session->m_socket, &event) == -1) {
                                close(session->m_socket);
                                session->notifyObserversClose();
                                release_session(session->m_socket);

                                mutex_map_sessions.unlock();
                                continue;
                            }
                            /*
                             * Все отлично начинаем подготавливать заголовок к отдачи
                             */
                            session->setStateResponse(new vms::StateResponsePrepareHeader(session));
                        }
                        mutex_map_sessions.unlock();
                    }
                    if (events[i].events & EPOLLOUT) {
                        mutex_map_sessions.lock();
                        vms::ISession * session = nullptr;
                        try {
                            session = map_sessions.at(events[i].data.fd);
                            if (session->getStateResponse()->id() == vms::STATE_RESPONSE_ID::STATE_RESPONSE_DESTROY_ID) {
                                close(session->m_socket);
                                session->notifyObserversClose();
                                release_session(session->m_socket);

                                mutex_map_sessions.unlock();
                                continue;
                            }
                            session->m_handler_response(session);
                        } catch (std::out_of_range & ex) {
                            LOGE << "Session not found id = " << events[i].data.fd;
                            mutex_map_sessions.unlock();
                            continue;
                        } catch (vms::session::exception::SessionException & ex) {
                            LOGE << ex.what();
                            mutex_map_sessions.unlock();
                            continue;
                        }
                        mutex_map_sessions.unlock();
                    }
                }
                free(events);
            }
        }
    }).detach();

    while (true) {
        client_socket = accept(serves_socket, 0, 0);
        if (client_socket == -1) {
            LOGE << "Error accept client socket. " << strerror(errno);
            continue;
        }
        int flags = fcntl(client_socket, F_GETFL, 0);
        if (flags < 0) {
            LOGE << "Could not get server socket. " << strerror(errno);
            close(client_socket);
            continue;
        }
        int err = fcntl(client_socket, F_SETFL, flags | O_NONBLOCK);
        if (err < 0) {
            LOGE << "Could set server socket to be non blocking. " << strerror(errno);
            close(client_socket);
            continue;
        }
        vms::ISession * session = new vms::Session(client_socket, 1024 * 1024);
        session->m_handler_response = [](vms::ISession * session) {
            session->getStateResponse()->handler_response();
        };
        session->m_handler_request = [](vms::ISession * session) {
            session->getStateRequset()->handler_request();
        };

        struct epoll_event event;
        event.data.fd = client_socket;
        event.events = EPOLLIN;
        mutex_map_sessions.lock();
        map_sessions[client_socket] = session;
        session->addObserverSessionClose(vms::Statistics::getInstance());

        if (epoll_ctl(epool_fd, EPOLL_CTL_ADD, client_socket, &event) == -1) {
            LOGE << "Error epool system set EPOLLIN";
            delete session;
            map_sessions.erase(client_socket);
            close(client_socket);
        } else
            session->setStateRequest(new vms::StateRequestReady(session));
        mutex_map_sessions.unlock();
        LOGD << "Create new connection id = " << client_socket;
    }
    return 0;
}

void setMaxOpenFiles() {
    int ret = getrlimit(RLIMIT_NOFILE, &resource_limits);
    if (ret == -1) {
        LOGE << "Error ser resource - max open filse";
        exit(EXIT_FAILURE);
    }
    resource_limits.rlim_cur = resource_limits.rlim_max;
    ret = setrlimit(RLIMIT_NOFILE, &resource_limits);
    if (ret == -1) {
        LOGE << "Error ser resource - max open filse";
        exit(EXIT_FAILURE);
    }
    LOGI << "Max count open files = " << resource_limits.rlim_max;
}