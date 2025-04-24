#include "HandlerViewVideoStream.h"

void vms::HandlerViewVideoStream::prepareData(ISession * session) {
    mutex_map_frames.lock();
    if (map_frames.find(m_channel) == map_frames.end()) {
        mutex_map_frames.unlock();
        throw (vms::session::exception::SessionException("Invalid parametrs"));
    }

    if (map_frames[m_channel].id > session->getIDFrame()) {
        session->setFrameID(map_frames[m_channel].id);
        std::string bytes_str = std::string(map_frames[m_channel].bytes.begin(), map_frames[m_channel].bytes.end());
        session->response_data = "--vms-stream\r\nContent-Type: image/jpeg\r\nContent-Length: " + std::to_string(bytes_str.size()) + "\r\n\r\n" + bytes_str + "\r\n";

    }
    mutex_map_frames.unlock();
}

void vms::HandlerViewVideoStream::prepareHeader(ISession * session) {
    session->response_data = "HTTP/1.0 200 OK\r\n"
            "Accept-Range: bytes\r\n"
            "Connection: keep-alive\r\n"
            "Max-Age: 0\r\n"
            "Expires: 0\r\n"
            "Cache-Control: no-cache, private\r\n"
            "Pragma: no-cache\r\n"
            "Content-Type: multipart/x-mixed-replace; boundary=vms-stream\r\n\r\n";
};