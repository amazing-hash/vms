#include "HandlerFetchJson.h"

vms::HandlerFetchJson::HandlerFetchJson(const json & msg) {
    m_msg = msg.dump();
}

void vms::HandlerFetchJson::prepareHeader(ISession* session) {
    session->response_data = "HTTP/1.1 200 OK\r\n"
            "Content-Length: " + std::to_string(m_msg.size()) + "\r\n"
            "Accept-Range: bytes\r\n"
            "Connection: close\r\n"
            "Content-type: pplication/json; charset=utf-8\r\n\r\n";
}

void vms::HandlerFetchJson::prepareData(ISession* session){
    session->response_data = m_msg;
}