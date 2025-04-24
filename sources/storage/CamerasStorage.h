#ifndef CAMERASSTORAGE_H
#define CAMERASSTORAGE_H

#include <fstream>
#include <mutex>

#include "../common/json/json.h"
#include "../common/plog/Log.h"

using json = nlohmann::json;

namespace vms {

    class CameraStorage {
    private:
        json m_json_cameras;
        static CameraStorage * m_instance;
        CameraStorage(){}
        std::mutex m_mutex;
    public:
        static CameraStorage * getInstance() noexcept {
            if (m_instance == nullptr)
                m_instance = new CameraStorage;
            return m_instance;
        }
        
        void read_cameras(const char * filename);
        
        json getJSON() {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_json_cameras;
        }
        
        void update();
        
    };
}

#endif

