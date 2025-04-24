#include "Statistics.h"

vms::Statistics * vms::Statistics::m_instance = nullptr;

void vms::Statistics::update(int fd) {
    
    int camera_id = m_relation_connection_and_camera[fd];
    m_connections[camera_id]--;
    auto it = m_relation_connection_and_camera.find(fd);
    if (it != m_relation_connection_and_camera.end())
        m_relation_connection_and_camera.erase(it);

}