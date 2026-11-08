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

# define _POSIX_C_SOURCE 200112L


int connect_to_server(const char *host, const char *port);
int send_all(int fd, const void *buffer, size_t length);
int send_header(int fd, uint64_t payload_length);



#endif