#ifndef ROUTES_H
#define ROUTES_H

#include "../session/TokenCollection.h"
#include "../session/IHandlerResponse.h"
#include "../common/plog/Log.h"
#include "../common/json/json.h"
#include "../common/mustache/mustache.h"
#include <string>
#include <mutex>
#include "../storage/CamerasStorage.h"

#include "../session/handlers/HandlerFetchCss.h"
#include "../session/handlers/HandlerFetchJs.h"
#include "../session/handlers/HandlerFetchSvg.h"
#include "../session/handlers/HandlerFetchJson.h"
#include "../session/handlers/HandlerFetchTemplateHtml.h"
#include "../session/handlers/HandlerViewVideoStream.h"

#include "../session/RequestFormParse.h"

#include "../statistics/Statistics.h"

#include "../session/ParamCollection.h"
#include "../session/ISession.h"

using json = nlohmann::json;

extern std::mutex mutex_configuration_file;

vms::IHandlerResponse* fetch_index_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);
vms::IHandlerResponse* fetch_statistics_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);
vms::IHandlerResponse* fetch_camera_view_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);
vms::IHandlerResponse* fetch_camera_configuration_page(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);

vms::IHandlerResponse* fetch_css_resources(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);
vms::IHandlerResponse* fetch_js_resources(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);
vms::IHandlerResponse* fetch_svg_resources(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);

vms::IHandlerResponse* save_camera_settings(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);

vms::IHandlerResponse* fetch_camera_stream(const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session);

#endif

