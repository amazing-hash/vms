#ifndef IHANDLERRESPONSE_H
#define IHANDLERRESPONSE_H

#include "ISession.h"

namespace vms {
    
    class ISession;
    
    class IHandlerResponse {
    public:

        virtual ~IHandlerResponse() {
        }

        virtual void prepareHeader(ISession * session) = 0;
        virtual void prepareData(ISession * session) = 0;
    };
}

#endif
