// socket.c

#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/limits.h>

#include "log.h"
#include "sockets.h"

#define SOCKET_TYPE    AF_UNIX

static struct sockaddr_un create_sockstruct(char * path)
{
    struct sockaddr_un s_addr;
    s_addr.sun_family = AF_UNIX;
    strncpy(s_addr.sun_path, path, sizeof(s_addr.sun_path) - 1);
    s_addr.sun_path[sizeof(s_addr.sun_path) - 1] = '\0';

    return s_addr;
}

static int create_socket(void)
{
    return socket(AF_UNIX, SOCK_STREAM, 0);
}

int init_server(char * path)
{
    struct sockaddr_un s_addr = create_sockstruct(path);

    int sock_fd = create_socket();

    if (sock_fd == -1)
    {
        return -1;
    }

    int ret = bind(sock_fd, (struct sockaddr*) &s_addr, sizeof(s_addr));

    if (ret == -1)
    {
        PERROR("bind");
        return -1;
    }
    ret = listen(sock_fd, SOCKET_BACKLOG);

    if (ret == -1)
    {
        PERROR("listen");
        return -1;
    }
    
    return sock_fd;
}

int wait_for_connection(int sock_fd)
{
    struct sockaddr_un s_addr;
    int s_addr_size = sizeof(s_addr);
    int ret = accept(sock_fd, (struct sockaddr*) &s_addr, (socklen_t*) &s_addr_size);
    
    if (ret == -1)
    {
	    PERROR("wait_for_connection() -- accept()");
	    return -1;
    }
    return ret;
}

int init_client(char * path)
{
    struct sockaddr_un s_addr = create_sockstruct(path);
    int sock_fd = create_socket();

    if (sock_fd == -1)
    {
	    PERROR("socket");
        return -1;
    }

    int ret = connect(sock_fd, (struct sockaddr*) &s_addr, sizeof(s_addr));

    if (ret == -1)
    {
	    PERROR("connect");
        return -1;
    }

    return sock_fd;
}

int close_socket(int sock_fd)
{
    return close(sock_fd);
}

int send_msg_socket(int sock_fd, void* msg, int msg_len)
{
    int amt_sent = 0;
    char * msg2 = (char*) msg;
    while (amt_sent < msg_len)
    {
        int ret = send(sock_fd, &msg2[amt_sent], msg_len - amt_sent, MSG_NOSIGNAL);

        if (ret == -1)
        {
	        PERROR("send");
            return -1;
        }

        amt_sent += ret;
    }

    return amt_sent;
}

int recv_msg_socket(int sock_fd, void * buf, int buf_len)
{
    int amt_recv = 0;
    char * buf2 = (char*) buf;

    while (amt_recv < buf_len)
    {
        int ret = recv(sock_fd, &buf2[amt_recv], buf_len - amt_recv, MSG_WAITALL);

        if (ret == -1)
        {
            PERROR("recv");
            return -1;
        }
        else if (ret == 0)
        {
            INFO("Client disconnected abruptly while waiting for message\n");
            return -1;
        }
        amt_recv += ret;
    }

    return amt_recv;
}


