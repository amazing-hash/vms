#ifndef STATEREQUESTIMPL_H
#define STATEREQUESTIMPL_H

#include "IStateRequest.h"
#include "../common/plog/Log.h"
#include "ISession.h"

#include "../exception/SessionException.h"

#include "StateResponseImpl.h"
#include "IHandlerResponse.h"
#include "Dispatcher.h"

#include "../parser/http/picohttpparser.h"

namespace vms {
    
    

    class StateRequestReceiveData : public IStateRequest {
    public:

        StateRequestReceiveData(ISession * session) : IStateRequest(session) {
        }

        virtual void handler_request() override;

        virtual STATE_REQUEST_ID id() {
            return STATE_REQUEST_ID::STATE_REQUEST_RECEIVE_DATA_ID;
        }

    };

    class StateRequestReady : public IStateRequest {
    public:

        StateRequestReady(ISession * session) : IStateRequest(session) {
        }

        virtual void handler_request() override {
            m_session->setStateRequest(new StateRequestReceiveData(m_session));
        }

        virtual STATE_REQUEST_ID id() {
            return STATE_REQUEST_ID::STATE_REQUEST_READY_ID;
        }
    };

    class StateRequestDestroy : public IStateRequest {
    public:

        StateRequestDestroy(ISession * session) : IStateRequest(session) {
        }

        virtual void handler_request() override {
            throw vms::session::exception::SessionException("Operation not supported this context");
        }

        virtual STATE_REQUEST_ID id() {
            return STATE_REQUEST_ID::STATE_REQUEST_DESTROY_ID;
        }
    };
}

#endif

