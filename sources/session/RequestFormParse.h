#ifndef REQUESTFORMPARSE_H
#define REQUESTFORMPARSE_H

#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

namespace vms {

    class RequestFormParse {
    private:

        static inline void ltrim(std::string &s) {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
                return !std::isspace(ch);
            }));
        }

        static inline void rtrim(std::string &s) {
            s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), s.end());
        }

    public:
        
        static inline void trim(std::string &s) {
            ltrim(s);
            rtrim(s);
        }

        static std::map<std::string, std::string> parse_form_params(const std::string& _body);
    };
}

#endif

