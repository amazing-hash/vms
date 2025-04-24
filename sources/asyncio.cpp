#include "asyncio.h"

int vms::read_async_socket(int socket, char * buffer, size_t len) {
        int ret = 0;
        int offset = 0;
        while (len != 0 && (ret = read(socket, buffer, len)) != 0) {
            if (ret == -1) {
                if (errno == EINTR)
                    continue;
                else if (errno == EAGAIN || errno == EWOULDBLOCK)
                    return offset;
                return -1;
            }
            offset += ret;
            len -= ret;
            buffer += ret;
        }
        if (!offset)
            return -1;
        return offset;
    }
    int vms::write_async_socket(int socket, char * buffer, size_t len) {
        int ret = 0;
        int offset = 0;
        while (len != 0 && (ret = write(socket, buffer, len)) != 0) {
            if (ret == -1) {
                if (errno == EINTR)
                    continue;
                else if (errno == EAGAIN || errno == EWOULDBLOCK)
                    return offset;
                return -1;
            }
            offset += ret;
            len -= ret;
            buffer += ret;
        }
        if (!offset)
            return -1;
        return offset;
    }