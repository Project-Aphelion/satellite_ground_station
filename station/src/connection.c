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
        sent = send(fd, data + total_sent, length, 0);
        if (sent <= 0)
            return (-1);
        if (sent > 0)
        {
            total_sent += sent;
            length -= sent;
        }
    }
    return (0);
}

int send_header(int fd, uint64_t payload_length)
{
// Put "GSP1" in the first 4 bytes.
// Set byte 5 to the protocol version (1).
// Convert the 64-bit payload length into big-endian (most significant byte first).
// Store those 8 bytes in the remaining positions of the buffer.
// Call your send_all() function to transmit the entire header.
// Return success or failure based on send_all().
    char header[13];
    memcpy(header, "GSP1", 4);
    header[4] = 1;
    int i = 0;
    int shift = 56;
    while (i < 8)
    {
        header[i + 5] = (payload_length >> (shift - (i * 8))) & 0xFF;
        i++;
    }
    if (send_all(fd, header, 13) < 0)
        return (-1);
    return (0);
}