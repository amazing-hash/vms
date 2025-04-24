#include "StateResponseImpl.h"

extern void release_session(int socket);

void vms::StateResponsePrepareHeader::handler_response() {
    m_session->getDelegateHandlerResponse()->prepareHeader(m_session);
    m_session->setDataWriteBuffer(m_session->response_data.c_str(), m_session->response_data.size());
    m_session->setStateResponse(new vms::StateResponseSendingHeader(m_session));
}

void vms::StateResponseSendingData::handler_response() {
    int ret = vms::write_async_socket(m_session->m_socket, m_session->m_pos_write_buffer, m_session->m_size_write_buffer - (m_session->m_pos_write_buffer - m_session->m_write_buffer));
    if (ret == -1) {
        LOGE << "Error write socket or connection is closed";
        m_session->setStateResponse(new StateResponseDestroy(m_session));
        return;
    }
    m_session->m_pos_write_buffer += ret;
    if (static_cast<unsigned int> (m_session->m_pos_write_buffer - m_session->m_write_buffer) == m_session->m_size_write_buffer) {
        delete [] m_session->m_write_buffer;
        m_session->m_write_buffer = nullptr;
        if (m_session->isAutoClose()) {
            close(m_session->m_socket);
            m_session->notifyObserversClose();
            release_session(m_session->m_socket);
            return;
        }
        m_session->setStateResponse(new vms::StateResponsePrepareData(m_session));
    }
}

void vms::StateResponsePrepareData::handler_response() {
    try {
        m_session->getDelegateHandlerResponse()->prepareData(m_session);
    } catch (vms::session::exception::SessionException & ex) {
        LOGE << ex.what();
        m_session->setStateResponse(new vms::StateResponseDestroy(m_session));
        return;
    }
    m_session->setDataWriteBuffer(m_session->response_data.c_str(), m_session->response_data.size());
    m_session->setStateResponse(new vms::StateResponseSendingData(m_session));
}

void vms::StateResponseSendingHeader::handler_response() {
    int ret = vms::write_async_socket(m_session->m_socket, m_session->m_pos_write_buffer, m_session->m_size_write_buffer - (m_session->m_pos_write_buffer - m_session->m_write_buffer));
    if (ret == -1) {
        LOGE << "Error write socket or connection is closed";
        close(m_session->m_socket);
        m_session->notifyObserversClose();
        release_session(m_session->m_socket);

        return;
    }
    m_session->m_pos_write_buffer += ret;
    if (static_cast<unsigned int> (m_session->m_pos_write_buffer - m_session->m_write_buffer) == m_session->m_size_write_buffer) {
        delete [] m_session->m_write_buffer;
        m_session->m_write_buffer = nullptr;
        if (m_session->isAutoClose()) {
            close(m_session->m_socket);
            m_session->notifyObserversClose();
            release_session(m_session->m_socket);

            return;
        }
        // Переводим в состояние отсылки данных
        m_session->setStateResponse(new vms::StateResponsePrepareData(m_session));

    }
}

void vms::StateResponseReady::handler_response() {
    m_session->setStateResponse(new vms::StateResponsePrepareHeader(m_session));
}