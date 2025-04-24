#ifndef STATISTICS_H
#define STATISTICS_H

#include <map>
#include <set>
#include "../session/ISession.h"
#include "../common/plog/Log.h"
#include <mutex>

namespace vms {
    
    class Statistics : public IEventSessionCLose{
    private:
        static Statistics * m_instance;
        Statistics(){};
    public:
        std::map<int, int> m_connections;
        std::map<int, int> m_relation_connection_and_camera;
        static Statistics * getInstance() noexcept {
            if (m_instance == nullptr)
                m_instance = new Statistics;
            return m_instance;
        }
        
        virtual void update(int fd) override;
        

    };
}

#endif

