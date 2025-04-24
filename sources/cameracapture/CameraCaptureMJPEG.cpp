#include <algorithm>
#include <opencv2/videoio.hpp>

#include "CameraCaptureMJPEG.h"

size_t vms::cameracapture::CameraCaptureMJPEG::MAX_SIZE_BUFFER = 4 * 1024 * 1024;
size_t vms::cameracapture::CameraCaptureMJPEG::TIME_BETWEEN_CONNECTION_ATTEMPTS = 5;

vms::cameracapture::CameraCaptureMJPEG::CameraCaptureMJPEG(const std::string & video_stream_address, const std::string & login, const std::string & password, int port) : m_video_stream_address(video_stream_address), m_login(login), m_password(password), m_port(port) {
    try {
        m_socket = std::unique_ptr<vms::socketio::SocketIO<unsigned char, char>>(new vms::socketio::SocketIO<unsigned char, char>(video_stream_address.substr(0, video_stream_address.find("/")), port));
        m_current_position_buffer = 0;
        m_header_accept = false;
        m_boundary = "--";
        m_buffer = std::unique_ptr<unsigned char>(new unsigned char [MAX_SIZE_BUFFER]);
        m_socket->connect_socket();
    } catch (vms::socketio::exception::SocketException & ex) {
        throw vms::cameracapture::exception::CameraCaptureException("Can't create CameraCapture for " + video_stream_address + ". " + std::string(strerror(errno)));
    }
    std::string login_password = login + ":" + password;
    std::string url;
    if(login != "" && password != "")
        url = {"GET /" + m_video_stream_address.substr(m_video_stream_address.find("/")) + " HTTP/1.0\r\nAuthorization: Basic " + vms::httpvideostream::base64_encode(login_password.c_str(), login_password.size()) + "\r\n\r\n"};
    else 
        url = {"GET /" + m_video_stream_address.substr(m_video_stream_address.find("/")) + "\r\n\r\n"};
    m_socket->write_socket_data(url.data(), url.size());
}

std::tuple<std::shared_ptr<std::vector<unsigned char>>, cv::Mat> vms::cameracapture::CameraCaptureMJPEG::fetchNextFrame() {
    unsigned char buffer[MAX_SIZE_INNER_BUFFER];
    unsigned char * pos_begin_image = nullptr;
    unsigned int size_img = 0;
    while (true) {
        int count = m_socket->read_socket_data(buffer, sizeof (buffer));
        if (count <= 0) {
            LOGW << "Try reconnect to camera " + m_video_stream_address;
            m_current_position_buffer = 0;
            m_header_accept = false;
            m_boundary = "--";
            memset(m_buffer.get(), 0, MAX_SIZE_INNER_BUFFER);
            m_socket.reset();
            sleep(TIME_BETWEEN_CONNECTION_ATTEMPTS);
            try {
                m_socket = std::unique_ptr<vms::socketio::SocketIO<unsigned char, char>>(new vms::socketio::SocketIO<unsigned char, char>(m_video_stream_address.substr(0, m_video_stream_address.find("/")), m_port));
                m_socket->connect_socket();
            } catch (vms::socketio::exception::SocketException & ex) {
                throw vms::cameracapture::exception::CameraCaptureException("Can't create CameraCapture for " + m_video_stream_address + ". " + std::string(strerror(errno)));
            }
            std::string login_password = m_login + ":" + m_password;
            std::string url;
            if (m_login != "" && m_password != "") 
                url = {"GET /" + m_video_stream_address.substr(m_video_stream_address.find("/")) + " HTTP/1.0\r\nAuthorization: Basic " + vms::httpvideostream::base64_encode(login_password.c_str(), login_password.size()) + "\r\n\r\n"};
            else
                url = {"GET /" + m_video_stream_address.substr(m_video_stream_address.find("/")) + "\r\n\r\n"};
            m_socket->write_socket_data(url.data(), url.size());
            continue;
        }
        memcpy(m_buffer.get() + m_current_position_buffer, buffer, count);
        m_current_position_buffer += count;
        if (m_current_position_buffer >= MAX_SIZE_BUFFER)
            throw vms::cameracapture::exception::CameraCaptureException("Buffer size exceeded. Camera " + m_video_stream_address);
        if (!m_header_accept) {
            unsigned char * pos_header = reinterpret_cast<unsigned char *> (strstr(reinterpret_cast<char*> (m_buffer.get()), "\r\n\r\n"));
            if (pos_header) {
                unsigned char *pos_boundary = reinterpret_cast<unsigned char *> (strstr(reinterpret_cast<char*> (m_buffer.get()), "boundary="));
                if (pos_boundary) {
                    pos_boundary += strlen("boundary=");
                    while (true) {
                        if (*pos_boundary == '\r' || *pos_boundary == '\n' || *pos_boundary == ' ')
                            break;
                        m_boundary.append(reinterpret_cast<char *> (pos_boundary), 1);
                        pos_boundary++;
                    }
                }
                m_header_accept = true;
            }
        }
        if (m_header_accept && pos_begin_image == nullptr) {
            pos_begin_image = reinterpret_cast<unsigned char *> (strstr(reinterpret_cast<char *> (m_buffer.get()), m_boundary.c_str()));
            if (pos_begin_image) {
                unsigned char *tmp = reinterpret_cast<unsigned char *> (strstr(reinterpret_cast<char *> (pos_begin_image), "Content-Length:"));
                if (tmp == nullptr) {
                    pos_begin_image = nullptr;
                } else {
                    size_img = atoi(reinterpret_cast<char *> (tmp) + strlen("Content-Length:"));
                    if (size_img == 0) {
                        pos_begin_image = nullptr;
                    }
                }
            }
        }
        if (m_header_accept && pos_begin_image != nullptr) {
            unsigned char *pos_begin_image_bytes = reinterpret_cast<unsigned char *> (strstr(reinterpret_cast<char *> (pos_begin_image), "\r\n\r\n"));
            if (pos_begin_image_bytes) {
                unsigned char *pos_end_image_bytes = reinterpret_cast<unsigned char *> (strstr(reinterpret_cast<char *> (pos_begin_image_bytes + 4 + size_img), m_boundary.c_str()));
                if (pos_end_image_bytes != nullptr) {
                    std::vector<unsigned char> * vec = new std::vector<unsigned char>(size_img);
                    for (size_t i = 0; i < size_img; i++)
                        (*vec)[i] = *(pos_begin_image_bytes + 4 + i);
                    memcpy(m_buffer.get(), pos_end_image_bytes, (m_buffer.get() + m_current_position_buffer) - pos_end_image_bytes);
                    m_current_position_buffer = (m_buffer.get() + m_current_position_buffer) - pos_end_image_bytes;
                    memset(m_buffer.get() + m_current_position_buffer, '\0', MAX_SIZE_BUFFER - m_current_position_buffer);
                    pos_begin_image = NULL;
                    cv::Mat mat;
                    if(m_decode_to_mat_opencv)
                        cv::imdecode(*vec, CV_LOAD_IMAGE_COLOR, &mat);
                    return std::make_tuple(std::shared_ptr<std::vector<unsigned char>>(vec), mat);
                }
            }
        }
    }

}