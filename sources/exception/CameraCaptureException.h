#ifndef CAMERAOUTPUTSTREAM_EXCEPTION_H
#define CAMERAOUTPUTSTREAM_EXCEPTION_H

#include <string>

namespace vms {
    namespace cameracapture {
        namespace exception {

            class CameraCaptureException : public std::exception {
            private:
                std::string m_message;
            public:

                CameraCaptureException(const std::string & message) :
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

