#ifndef HANDLERFETCHHTML_H
#define HANDLERFETCHHTML_H

#include "../IHandlerResponse.h"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "../../common/plog/Log.h"
#include "../../exception/SessionException.h"

namespace vms {
    class HandlerFetchHtml : public IHandlerResponse {
    private:
        std::string m_page;
        
        std::string fileReader();
        
    public:
        HandlerFetchHtml(const std::string & page) : m_page(page){
            
        }
        virtual void prepareHeader(ISession * session) override;
        virtual void prepareData(ISession * session) override;
    };
}

#endif

