#ifndef CONNECTION_H
# define CONNECTION_H

# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netdb.h>
# include <string.h>
# include <errno.h>
# include <sys/stat.h>
# include <fcntl.h>

# define _POSIX_C_SOURCE 200112L


// connection.c
int connect_to_server(const char *host, const char *port);

// send.c
int send_all(int fd, const void *buffer, size_t length);
int send_header(int fd, uint64_t payload_length);
int send_file(int server_fd, const char *filepath)


#endif