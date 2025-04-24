#ifndef HANDLERFETCHCSS_H
#define HANDLERFETCHCSS_H

#include "../IHandlerResponse.h"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "../../common/plog/Log.h"
#include "../../exception/SessionException.h"

namespace vms {
    class HandlerFetchCss : public IHandlerResponse {
    private:
        std::string m_file;
        
        std::string fileReader();
        
    public:
        HandlerFetchCss(const std::string & file) : m_file(file){
            
        }
        virtual void prepareHeader(ISession * session) override;
        virtual void prepareData(ISession * session) override;
    };
}

#endif

