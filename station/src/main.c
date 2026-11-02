#include "../include/connection.h"

int main(int ac, char **av)
{
	if (ac != 3)
		return (printf("Usage: %s <host> <port>\n", av[0]));
	printf("Connecting to %s:%s...\n", av[1], av[2]);
	return (0);
}