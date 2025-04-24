#ifndef HANDLERFETCHTEMPLATEHTML_H
#define HANDLERFETCHTEMPLATEHTML_H

#include "../IHandlerResponse.h"
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "../../common/plog/Log.h"
#include "../../exception/SessionException.h"

namespace vms {
    class HandlerFetchTemplateHtml : public IHandlerResponse {
    private:
        std::string m_template;
        
    public:
        HandlerFetchTemplateHtml(const std::string & tmpl) : m_template(tmpl){
            
        }
        virtual void prepareHeader(ISession * session) override;
        virtual void prepareData(ISession * session) override;
    };
}

#endif

