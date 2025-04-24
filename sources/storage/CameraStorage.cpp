#include "CamerasStorage.h"

vms::CameraStorage * vms::CameraStorage::m_instance = nullptr;

void vms::CameraStorage::read_cameras(const char * filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOGE << "File cameras '" << filename << " ' can't opened";
        exit(EXIT_FAILURE);
    }
    std::string str;
    std::stringstream ss;
    while (file >> str) {
        ss << str;
    }
    try {
        m_json_cameras = json::parse(ss.str());
    } catch (...) {
        LOGE << "Invalid format file " << filename;
        exit(EXIT_FAILURE);
    }
}

void vms::CameraStorage::update(){
    
}