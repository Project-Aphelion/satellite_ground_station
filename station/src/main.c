#include "../include/connection.h"

int main(int ac, char **av)
{
	int	connected_fd = -1;
	if (ac != 3)
		return (printf("Usage: %s <host> <port>\n", av[0]), -1);
	

	printf("Connecting to %s:%s...\n", av[1], av[2]);
	connected_fd = connect_to_server(av[1], av[2]);
	if (connected_fd < 0)
		return (printf("Connection failed\n"), -1);
	send(connected_fd, "Hello Anakin!\n", 14, 0);
	return (0);
}