#ifndef ASYNCIO_H
#define ASYNCIO_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>



namespace vms {

    /* 
     * Read socket
     * Return position of buffer
     * Return -1 error read from socket or close connection
     */
    int read_async_socket(int socket, char * buffer, size_t len);

    /* 
     * Write socket
     * Return position of buffer
     * Return -1 error read from socket or close connection
     */
    int write_async_socket(int socket, char * buffer, size_t len);
}

#endif

