#include "../include/connection.h"

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

int send_file(int server_fd, const char *filepath)
{
    struct stat st;
    char buffer[4096];
    ssize_t total_sent = 0;
    int bytes_read;

    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0)
        return (-1);
    fstat(file_fd, &st);
    if (send_header(server_fd, st.st_size) < 0)
        return (-1); //error handle
    
    while (total_sent < st.st_size)
    {
        bytes_read = read(file_fd, buffer, 4096);
        if (send_all(server_fd, buffer, bytes_read) < 0)
            return (-1); // do proper error handling here ig.
        total_sent += bytes_read;
    }
    close(file_fd);
    return (0);
}