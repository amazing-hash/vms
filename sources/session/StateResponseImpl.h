#ifndef STATERESPONSEIMPL_H
#define STATERESPONSEIMPL_H

#include "../common/plog/Log.h"
#include "ISession.h"
#include "IStateResponse.h"
#include "../exception/SessionException.h"
#include "../asyncio.h"
#include "../statistics/Statistics.h"

namespace vms {

    class StateResponseNotReady : public IStateResponse {
    public:

        StateResponseNotReady(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override {
            throw vms::session::exception::SessionException("Operation not supported this context");
        }

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_NOT_READY_ID;
        }
    };

    class StateResponseReady : public IStateResponse {
    public:

        StateResponseReady(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override;

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_READY_ID;
        }
    };

    class StateResponsePrepareHeader : public IStateResponse {
    public:

        StateResponsePrepareHeader(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override;

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_PREPARE_HEADER_ID;
        }
    };

    class StateResponseSendingHeader : public IStateResponse {
    public:

        StateResponseSendingHeader(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override;

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_SENDING_HEADER_ID;
        }
    };

    class StateResponsePrepareData : public IStateResponse {
    public:

        StateResponsePrepareData(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override;

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_PREPARE_DATA_ID;
        }
    };

    class StateResponseDestroy : public IStateResponse {
    public:

        StateResponseDestroy(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override {
            throw vms::session::exception::SessionException("Operation not supported this context");
        }

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_DESTROY_ID;
        }
    };

    class StateResponseSendingData : public IStateResponse {
    public:

        StateResponseSendingData(ISession * session) : IStateResponse(session) {
        }

        virtual void handler_response() override;

        virtual STATE_RESPONSE_ID id() {
            return STATE_RESPONSE_ID::STATE_RESPONSE_SENDING_DATA_ID;
        }
    };
}

#endif

