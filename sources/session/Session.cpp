#include "Session.h"

vms::Session::Session(int socket, size_t size_read_buffer) {
    m_socket = socket;
    m_size_read_buffer = size_read_buffer;
    m_read_buffer = new char[size_read_buffer];
    m_pos_read_buffer = m_read_buffer;
    memset(m_read_buffer, '\0', size_read_buffer);
    // Начинаем с состояния, что данные еще не получены от клиента и мы не готовы для отдачи данных
    setStateResponse(new StateResponseNotReady(this));
}

void vms::Session::setDataWriteBuffer(const char * data, int size) {
    m_write_buffer = new char[size + 1];
    m_size_write_buffer = size;
    memset(m_write_buffer, '\0', size + 1);
    memcpy(m_write_buffer, data, size);
    m_pos_write_buffer = m_write_buffer;
}
