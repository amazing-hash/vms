#ifndef ISTATERESPONSE_H
#define ISTATERESPONSE_H

#include "ISession.h"

namespace vms {
    
    class ISession;
    
    enum class STATE_RESPONSE_ID {
        STATE_RESPONSE_NOT_READY_ID, STATE_RESPONSE_READY_ID, STATE_RESPONSE_PREPARE_HEADER_ID, STATE_RESPONSE_SENDING_HEADER_ID, STATE_RESPONSE_PREPARE_DATA_ID, STATE_RESPONSE_SENDING_DATA_ID, STATE_RESPONSE_DESTROY_ID
    };
    
    class IStateResponse {
    protected:
        ISession * m_session = nullptr;
    public:

        IStateResponse(ISession * session) {
            m_session = session;
        }

        virtual ~IStateResponse() {
        }
        virtual void handler_response() = 0;
        virtual STATE_RESPONSE_ID id() = 0;

    };
}


#endif

