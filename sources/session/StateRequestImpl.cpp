#include "StateRequestImpl.h"

void vms::StateRequestReceiveData::handler_request() {
    int ret = vms::read_async_socket(m_session->m_socket, m_session->m_pos_read_buffer, m_session->m_size_read_buffer - (m_session->m_pos_read_buffer - m_session->m_read_buffer));
    if (ret == -1) {
        LOGE << "Error read socket or connection is closed";
        m_session->setStateRequest(new StateRequestDestroy(m_session));
        return;
    }
    unsigned int prev_buf_len = m_session->m_pos_read_buffer - m_session->m_read_buffer;
    if (static_cast<unsigned int> (m_session->m_pos_read_buffer - m_session->m_read_buffer + ret) < m_session->m_size_read_buffer)
        m_session->m_pos_read_buffer += ret;

    char * method, * path;
    int minor_version;
    struct phr_header headers[100];
    size_t method_len, path_len, num_headers;
    num_headers = sizeof (headers) / sizeof (headers[0]);
    ret = phr_parse_request(m_session->m_read_buffer, m_session->m_pos_read_buffer - m_session->m_read_buffer, (const char **) &method, &method_len, (const char **) &path, &path_len, &minor_version, headers, &num_headers, prev_buf_len);
    if(ret > 0){
        m_session->request_data = std::string(m_session->m_read_buffer, m_session->m_pos_read_buffer);
        try {
            // Даные все получены, мы готовы к передаче данных
            vms::IHandlerResponse * handler_response = vms::Dispatcher::dispatcher(m_session);
            m_session->setStateResponse(new vms::StateResponseReady(m_session));
            m_session->setDelegateHandlerResponse(handler_response);
        } catch (vms::session::exception::SessionException & ex) {
            LOGE << ex.what();
            m_session->setStateRequest(new StateRequestDestroy(m_session));
            m_session->setStateResponse(new StateResponseDestroy(m_session));
        }
    } 
    if (ret == -1) {
        m_session->setStateRequest(new StateRequestDestroy(m_session));
        m_session->setStateResponse(new StateResponseDestroy(m_session));
    } 

}