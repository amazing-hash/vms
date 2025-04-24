#ifndef ISTATEREQUEST_H
#define ISTATEREQUEST_H

#include "ISession.h"

namespace vms {
    
    class ISession;
    
    enum class STATE_REQUEST_ID {
        STATE_REQUEST_READY_ID, STATE_REQUEST_RECEIVE_DATA_ID, STATE_REQUEST_DESTROY_ID
    };
    
    class IStateRequest {
    protected:
        ISession * m_session = nullptr;
    public:

        IStateRequest(ISession * session) {
            m_session = session;
        }

        virtual ~IStateRequest() {
        }
        virtual void handler_request() = 0;
        virtual STATE_REQUEST_ID id() = 0;

    };
}

#endif

