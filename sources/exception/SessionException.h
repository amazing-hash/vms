#ifndef SESSIONEXCEPTION_H
#define SESSIONEXCEPTION_H

#include <string>

namespace vms {
    namespace session {
        namespace exception {

            class SessionException : public std::exception {
            private:
                std::string m_message;
            public:

                SessionException(const std::string & message) :
                exception(), m_message(message) {
                }

                virtual const char* what() const throw () override {
                    return m_message.c_str();
                }
            };
        }
    }
}

#endif

