#ifndef HANDLERFETCHJSON_H
#define HANDLERFETCHJSON_H

#include "../IHandlerResponse.h"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "../../common/plog/Log.h"
#include "../../exception/SessionException.h"

#include "../../common/json/json.h"

using json = nlohmann::json;

namespace vms {
    class HandlerFetchJson : public IHandlerResponse {
    private:
        std::string m_msg;
        
        
    public:
        HandlerFetchJson(const json & msg);
        virtual void prepareHeader(ISession * session) override;
        virtual void prepareData(ISession * session) override;
    };
}

#endif

