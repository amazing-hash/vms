#include "HandlerFetchTemplateHtml.h"

void vms::HandlerFetchTemplateHtml::prepareHeader(ISession* session) {
    session->response_data = "HTTP/1.1 200 OK\r\n"
            "Content-Length: " + std::to_string(m_template.size()) + "\r\n"
            "Accept-Range: bytes\r\n"
            "Connection: close\r\n"
            "Content-type: text/html; charset=utf-8\r\n\r\n";
}

void vms::HandlerFetchTemplateHtml::prepareData(ISession* session){
    session->response_data = m_template;
    session->m_auto_close = true;
}