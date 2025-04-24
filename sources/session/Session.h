#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <string.h>
#include <functional>
#include <unistd.h>
#include <mutex>
#include <memory>
#include <map>

#include "../exception/SessionException.h"
#include "../asyncio.h"
#include "../common/plog/Log.h"
#include "../frame.h"
#include "../parser/http/picohttpparser.h"

#include "ISession.h"
#include "IStateResponse.h"
#include "IStateRequest.h"
#include "IHandlerResponse.h"
#include "StateResponseImpl.h"


extern std::mutex mutex_map_sessions;

extern std::map<int, Frame> map_frames;
extern std::mutex mutex_map_frames;

namespace vms {
    
    class IStateResponse;
    
    class Session : public ISession {
        
    public:

        bool isAutoClose() const {
            return m_auto_close;
        }

        unsigned long long getIDFrame() const {
            return m_frame_id;
        }

        void setAutoClose(bool state) {
            m_auto_close = state;
        }

        void setFrameID(unsigned long long id) {
            m_frame_id = id;
        }

        void setDelegateHandlerResponse(IHandlerResponse * handler) override {
            m_delegate_handler_response = handler;
        }
        
        IHandlerResponse * getDelegateHandlerResponse() override {
            return m_delegate_handler_response;
        }

        void setDataWriteBuffer(const char * data, int size) override;

        Session(const Session&) = delete;
        void operator=(const Session&) = delete;
        Session(const Session&&) = delete;
        void operator=(const Session&&) = delete;

        Session(int socket, size_t size_read_buffer = 0);

        void setStateResponse(IStateResponse * state) {
            delete m_state_response;
            m_state_response = state;
        }

        IStateResponse * getStateResponse() {
            return m_state_response;
        }

        void setStateRequest(IStateRequest * state) {
            delete m_state_request;
            m_state_request = state;
        }

        IStateRequest * getStateRequset() {
            return m_state_request;
        }

        ~Session() override {
            delete [] m_read_buffer;
            delete [] m_write_buffer;
            delete m_state_request;
            delete m_state_response;
            delete m_delegate_handler_response;
        }
    };
    
    

}

extern std::map<int, vms::ISession *> map_sessions;


#endif

