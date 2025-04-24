#ifndef TOKENCOLLECTION_H
#define TOKENCOLLECTION_H

#include <string>
#include <vector>
#include <memory>
#include <string.h>

#include "../exception/SessionException.h"

namespace vms {

    class TokenCollection {
    private:
        std::vector<std::string> m_tokens;
    public:
        TokenCollection(const std::string & url);

        std::string & operator[](size_t index) {
            if (index > m_tokens.size() || index < 0)
                throw vms::session::exception::SessionException("Index doesn't exists in the TokenCollection");
            return m_tokens[index];
        }

        int size() {
            return m_tokens.size();
        }
    };
}

#endif

