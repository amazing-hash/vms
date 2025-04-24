#include "RequestFormParse.h"

std::map<std::string, std::string> vms::RequestFormParse::parse_form_params(const std::string & _body) {
    std::string body(_body);
    trim(body);
    std::map<std::string, std::string> map_form_params;
    std::unique_ptr<char[] > buffer(new char[body.size() + 1]);
    memset(buffer.get(), '\0', body.size() + 1);
    strncpy(buffer.get(), body.c_str(), body.size() + 1);
    char *savep_tr;
    char *token = strtok_r(buffer.get(), "&", &savep_tr);
    std::vector<std::string> tmp;
    while (token != NULL) {
        tmp.push_back(token);
        token = strtok_r(NULL, "&", &savep_tr);
    }
    for (size_t i = 0; i < tmp.size(); i++) {
        std::string value = tmp[i];
        size_t sep = value.find("=");
        std::string key, val;
        key = value.substr(0, sep);
        val = value.substr(sep + 1, value.size() - sep - 1);
        map_form_params.insert(std::pair<std::string, std::string>(key, val));
    }
    return map_form_params;
}