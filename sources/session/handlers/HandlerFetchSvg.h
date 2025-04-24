#ifndef HANDLERFETCHSVG_H
#define HANDLERFETCHSVG_H

#include "../IHandlerResponse.h"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "../../common/plog/Log.h"
#include "../../exception/SessionException.h"

namespace vms {
    class HandlerFetchSvg : public IHandlerResponse {
    private:
        std::string m_file;
        
        std::string fileReader();
        
    public:
        HandlerFetchSvg(const std::string & file) : m_file(file){
            
        }
        virtual void prepareHeader(ISession * session) override;
        virtual void prepareData(ISession * session) override;
    };
}

#endif

