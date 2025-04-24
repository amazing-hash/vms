#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "IHandlerResponse.h"
#include "../parser/http/picohttpparser.h"
#include <string.h>
#include <memory>

#include "handlers/HandlerViewVideoStream.h"
#include "ISession.h"
#include "../common/plog/Log.h"
#include "../common/json/json.h"
#include "../common/mustache/mustache.h"
#include "TokenCollection.h"

#include "handlers/HandlerFetchHtml.h"
#include "TokenCollection.h"
#include "handlers/HandlerFetchJs.h"
#include "handlers/HandlerFetchCss.h"
#include "handlers/HandlerFetchSvg.h"
#include "handlers/HandlerFetchTemplateHtml.h"

#include "../statistics/Statistics.h"

#include "ParamCollection.h"

#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <string>

#include <regex>
#include <algorithm>
#include <set>
#include <map>
#include <functional>
#include <vector>

#include "../storage/CamerasStorage.h"

using json = nlohmann::json;

extern json json_cameras;

namespace vms {

    class Dispatcher {
    private:
        struct Route {
            std::regex re;
            std::function<IHandlerResponse*(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, ISession * session)> handler;
        };
        static std::vector<Route> m_routes_post;
        static std::vector<Route> m_routes_get;
        
        static void urldecode(char * st);
        
    public:
        static IHandlerResponse * dispatcher(ISession * session);
        static void addRoute(const std::string & regex, const std::string & method, std::function<IHandlerResponse*(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, ISession * session)> route);    };

}

#endif

