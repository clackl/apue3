#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fd, i;
	char buf[4096];

	if (argc < 2) {
		printf("Enter like this: ./a.out fifoname\n");
		return -1;
	}
	fd = open(argv[1], O_WRONLY);
	if (fd < 0) {
		perror("open error");
		return -1;
	}

	i = 0;
	while (1) {
		sprintf(buf, "hello itcast %d\n", i++);

		write(fd, buf, strlen(buf));
		sleep(1);
	}
	close(fd);

	return 0;
}
