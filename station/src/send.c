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

    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0)
        return (-1);
    fstat(file_fd, &st);
    send_header(server_fd, st.st_size);
}
// 3. The structure of send_file()
// I'll leave the implementation to you, but here's the pseudocode:
// 1. Open the file using open().
// 2. Call fstat() to determine its size. Check for errors and enforce our 256 MiB protocol limit.
// 3. Send the GSP1 header containing that size.
// 4. Create a 4096-byte buffer.
// 5. Repeatedly read() into that buffer and call send_all() with the actual number of bytes read.
// 6. Keep a running total, and stop once you've sent the declared file size.
// 7. Close the file and return success or failure.