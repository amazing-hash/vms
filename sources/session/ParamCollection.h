#ifndef PARAMCOLLECTION_H
#define PARAMCOLLECTION_H

#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <string>
#include <string.h>
#include <memory>
#include "../exception/SessionException.h"
#include "../common/plog/Log.h"

namespace vms {

    class ParamCollection {
    private:
        std::map<std::string, std::string> m_params;
    public:
        ParamCollection(const std::string & params);
        ParamCollection(){};

        std::string & operator[](const std::string & key) {
            auto it = m_params.find(key);
            if (it == m_params.end())
                throw vms::session::exception::SessionException("Key " + key + " doesn't exists in the ParamCollection");
            return it->second;
        }

        bool has(const std::string & key) {
            auto it = m_params.find(key);
            if (it == m_params.end())
                return false;
            return true;
        }

        int size() {
            return m_params.size();
        }


    };
}

#endif

