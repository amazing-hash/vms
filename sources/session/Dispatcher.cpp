#include "Dispatcher.h"

std::vector<vms::Dispatcher::Route> vms::Dispatcher::m_routes_post;
std::vector<vms::Dispatcher::Route> vms::Dispatcher::m_routes_get;

vms::IHandlerResponse * vms::Dispatcher::dispatcher(ISession * session) {

    const char * data = session->request_data.c_str();
    char * method, * path;
    int minor_version;
    struct phr_header headers[100];
    size_t method_len, path_len, num_headers;
    num_headers = sizeof (headers) / sizeof (headers[0]);
    phr_parse_request(data, strlen(session->request_data.c_str()), (const char **) &method, &method_len, (const char **) &path, &path_len, &minor_version, headers, &num_headers, 0);
    
    std::unique_ptr<char[] > method_unique_ptr(new char[method_len + 1]);
    memset(method_unique_ptr.get(), 0, method_len + 1);
    strncpy(method_unique_ptr.get(), method, method_len);

    std::unique_ptr<char[] > path_unique_ptr(new char[path_len + 1]);
    memset(path_unique_ptr.get(), 0, path_len + 1);
    strncpy(path_unique_ptr.get(), path, path_len);

    vms::TokenCollection tokens(path_unique_ptr.get());

    std::smatch match;

    std::string path_str = std::string(path_unique_ptr.get());

    vms::ParamCollection params;
    int pos = path_str.find_first_of("?");
    if (pos != std::string::npos)
        params = vms::ParamCollection(path_str.substr(pos + 1));

    if (strcmp(method_unique_ptr.get(), "GET") == 0) {
        for (vms::Dispatcher::Route & route : m_routes_get) {
            if (std::regex_match(path_str, match, route.re)) {
                return route.handler(path_str, method_unique_ptr.get(), tokens, params, "", session);
            }
        }
    } else if (strcmp(method_unique_ptr.get(), "POST") == 0) {
        for (vms::Dispatcher::Route & route : m_routes_post) {
            if (std::regex_match(path_str, match, route.re)) {
                urldecode(const_cast<char *>(data));
                std::string body(data);
                body = body.substr(body.find("\r\n\r\n"), std::string::npos);
                return route.handler(path_str, method_unique_ptr.get(), tokens, params, body, session);
            }
        }
    }
    throw vms::session::exception::SessionException("Processing error in the dispatcher. No matches found url " + std::string(path_unique_ptr.get()));
}

void vms::Dispatcher::addRoute(const std::string & regex, const std::string & method, std::function < IHandlerResponse * (const std::string & url, const std::string & method, vms::TokenCollection & tokens, vms::ParamCollection & params, const std::string & body, vms::ISession * session) > route) {
    if (method == "POST") {
        std::regex re(regex);
        m_routes_post.push_back({re, route});
        return;
    } else if (method == "GET") {
        std::regex re(regex);
        m_routes_get.push_back({re, route});
        return;
    }
    LOGE << "Unsupported method " << method;
    exit(EXIT_FAILURE);
}

void vms::Dispatcher::urldecode(char * st) {
    char * p = st;
    char hex[3];
    int code;
    do {
        if (*st == '%') {
            hex[0] = *(++st);
            hex[1] = *(++st);
            hex[2] = 0;
            sscanf(hex, "%X", &code);
            *p++ = (char) code;
        } else if (*st == '+') *p++ = ' ';
        else *p++ = *st;
    } while (*st++ != 0);
}