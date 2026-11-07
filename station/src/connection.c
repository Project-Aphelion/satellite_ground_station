#include "../include/connection.h"

int connect_to_server(const char *host, const char *port)
{
    struct  addrinfo hints = {0};
    struct  addrinfo *result = NULL;
    struct  addrinfo *p;
    int     fd;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, port, &hints, &result);
    if (status) // returns nonzero on error
        return (printf("getaddrinfo: %s\n", gai_strerror(status)), -1);

    for (p = result; p != NULL; p = p->ai_next)
    {
        fd = socket(
        p->ai_family,
        p->ai_socktype,
        p->ai_protocol
        );
        status = connect(fd, p->ai_addr, p->ai_addrlen);
        if (status < 0) // bad connect
        {
            close(fd);
            continue ;
        }
        if (status == 0) // good connection
        {
            printf("Successfully connected! fd = %d\n", fd);
            break ;
        }
        // close on failure
    }
    return (fd);
}

int send_all(int fd, const void *buffer, size_t length)
{
    const unsigned char *data = buffer;
    size_t total_sent = 0;
    ssize_t sent;

    while (total_sent < length)
    {
        // Call send() with:
        send(fd, "Hello Anakin!\n", 14, 0);
        // - fd
        // - data + total_sent
        // - length - total_sent
        // - flags

        // If sent > 0, update total_sent
        // If interrupted (EINTR), retry
        // Otherwise, return -1
    }
    return (0);
}

int send_header(int fd, uint64_t payload_length)
{
    
}