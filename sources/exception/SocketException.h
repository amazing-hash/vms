#ifndef SOCKET_EXCEPTION_H
#define SOCKET_EXCEPTION_H

#include <string>

namespace vms {
    namespace socketio {
        namespace exception {

            class SocketException : public std::exception {
            private:
                std::string m_message;
            public:

                SocketException(const std::string & message) :
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

