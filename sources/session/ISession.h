#ifndef ISESSION_H
#define ISESSION_H

#include <map>
#include <vector>
#include <mutex>

#include "IStateResponse.h"
#include "IHandlerResponse.h"
#include "IStateRequest.h"

extern std::map<int, vms::ISession *> map_sessions;
extern std::mutex mutex_map_sessions;


namespace vms {
    
    class IEventSessionCLose {
    public:
        virtual void update(int fd) = 0;
        virtual ~IEventSessionCLose(){}
    };

    class IHandlerResponse;
    class IStateRequest;
    class IStateResponse;

    class ISession {
    private:
        std::vector<IEventSessionCLose *> observers_close;
    public:
        
        int m_socket;
        
        void addObserverSessionClose(IEventSessionCLose * observer){
            observers_close.push_back(observer);
        }
        
        void notifyObserversClose(){
            for(auto observer : observers_close)
                observer->update(m_socket);
        }

        IStateResponse * m_state_response = nullptr;
        IStateRequest * m_state_request = nullptr;

        IHandlerResponse * m_delegate_handler_response = nullptr;

        bool m_auto_close = false;
        unsigned long long m_frame_id = 0;

        char * m_read_buffer = nullptr;
        char * m_pos_read_buffer;

        char * m_write_buffer = nullptr;
        char * m_pos_write_buffer;

        size_t m_size_read_buffer;
        size_t m_size_write_buffer;

        std::string request_data;
        std::string response_data;

        virtual ~ISession() {
            notifyObserversClose();
        };

        std::function<void(ISession * session) > m_handler_request = nullptr;
        std::function<void(ISession * session) > m_handler_response = nullptr;

        virtual void setDataWriteBuffer(const char * data, int size) = 0;

        virtual bool isAutoClose() const = 0;

        virtual unsigned long long getIDFrame() const = 0;

        virtual void setAutoClose(bool state) = 0;

        virtual void setFrameID(unsigned long long id) = 0;

        virtual void setDelegateHandlerResponse(IHandlerResponse * handler) = 0;

        virtual IHandlerResponse * getDelegateHandlerResponse() = 0;

        virtual void setStateResponse(IStateResponse * state) = 0;

        virtual IStateResponse * getStateResponse() = 0;

        virtual void setStateRequest(IStateRequest * state) = 0;

        virtual IStateRequest * getStateRequset() = 0;

    };
}


#endif

