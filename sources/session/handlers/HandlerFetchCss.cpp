#include <locale>

#include "HandlerFetchCss.h"

std::string vms::HandlerFetchCss::fileReader() {
    std::stringstream buffer;
    std::ifstream file(m_file, std::ios::binary);
    if (file.is_open()) {
        while (file.good()) {
            char buf[4096];
            memset(buf, 0, sizeof(buf));
            file.read((char*)&buf, sizeof(buf) - 1);  
            buffer << buf;
        }
        file.close();
        return buffer.str();
    }
    throw vms::session::exception::SessionException("File not found " + m_file);
}
void vms::HandlerFetchCss::prepareHeader(ISession* session) {
    struct stat buff;
    stat(m_file.c_str(), &buff);
    session->response_data = "HTTP/1.1 200 OK\r\n"
            "Content-Length: " + std::to_string(buff.st_size) + "\r\n"
            "Accept-Range: bytes\r\n"
            "Connection: close\r\n"
            "Content-type: text/css; charset=utf-8\r\n\r\n";
}

void vms::HandlerFetchCss::prepareData(ISession* session){
    session->response_data = fileReader();
    session->m_auto_close = true;
}