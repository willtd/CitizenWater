/*
 *  sockets.h
 */

#ifndef SOCKETS_H_

#define SOCKETS_H_

#define SOCKET_BACKLOG 50

/* Initialize an AF_UNIX socket on the given path.
 * It is the callers responsibility to ensure that the path does not currently 
 * exist. Returns the socket file descriptor on success and -1 on error.
 */
int init_server(char * path);

/*
 * Waits for a client to connect to the given socket file descriptor.
 * Blocks until a client arrives.
 */
int wait_for_connection(int sock_fd);

/*
 * Initializes a client socket on the given path. The server socket 
 * should be setup prior to calling this function.
 * Returns the socket file descriptor on success and -1 on error.
 */
int init_client(char * path);

/* 
 * Closes the given socket path.
 */
int close_socket(int sock_fd);

/*
 * Reads a message of msg_len bytes into the msg pointer.
 * Returns the number of bytes read on success, and -1 on failure.
 */
int send_msg_socket(int sock_fd, void* msg, int msg_len);

/* 
 * Sends a message of buf_len bytes from the buf pointer.
 * Returns the number of bytes sent on success, and -1 on failure.
 */
int recv_msg_socket(int sock_fd, void* buf, int buf_len);

#endif

