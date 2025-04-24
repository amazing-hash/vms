#ifndef CAPTURE_MJPEG_SOCKET_H
#define CAPTURE_MJPEG_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <fcntl.h>
#include <cerrno>
#include <poll.h>

#include <sys/ioctl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <exception>

#include "../exception/SocketException.h"

namespace vms {
    namespace socketio {

        template <typename T, typename V> class SocketIO {
            int32_t m_sock;
            struct sockaddr_in m_cli_addr;

        public:

            SocketIO(const std::string & address, int port) {
                m_sock = socket(AF_INET, SOCK_STREAM, 0);
                if (m_sock < 0)
                    throw vms::socketio::exception::SocketException("Can't create socket." + std::string(strerror(errno)));
                m_cli_addr.sin_family = AF_INET;
                m_cli_addr.sin_addr.s_addr = inet_addr(address.c_str());
                m_cli_addr.sin_port = htons(port);
                
                struct timeval t;
                t.tv_sec = 5;
                t.tv_usec = 0;
                setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof (t));
            }

            ~SocketIO() {
                shutdown(m_sock, SHUT_RDWR);
            }

            void connect_socket() {
                int status = connect(m_sock, (struct sockaddr *) &m_cli_addr, sizeof (m_cli_addr));
                if (status == -1) {
                    close(m_sock);
                    throw vms::socketio::exception::SocketException("Can't create connect. " + std::string(strerror(errno)));
                }
            }

            int read_socket_data(T* buf, size_t len) noexcept {
                memset(buf, 0, len);
                ssize_t ret, count = len;
                while (len != 0 && (ret = read(m_sock, buf, len)) != 0) {
                    if (ret == -1) {
                        if (errno == EINTR)
                            continue;
                        return -1;
                    }
                    len -= ret;
                    buf += ret;
                }
                return count - len;
            }

            int write_socket_data(const V* buf, size_t len) noexcept {
                ssize_t ret, count = len;
                while (len != 0 && (ret = write(m_sock, buf, len)) != 0) {
                    if (ret == -1) {
                        if (errno == EINTR)
                            continue;
                        return -1;
                    }
                    len -= ret;
                    buf += ret;
                }
                return count - len;
            };

            int32_t getSock() const {
                return m_sock;
            }


        };


    }
}

#endif

