#include "apue.h"
#include <fcntl.h>

int mainI0()
{
	if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
		printf("cannot seek\n");
	else
		printf("seek OK\n");
	exit(0);
}

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";
int mainI1()
{
	int fd;
	if ((fd = creat("file.hole", FILE_MODE)) < 0)
		err_sys("create err");
	if (write(fd, buf1, 10) != 10)
		err_sys("buf1 write err");
	if (lseek(fd, 163, SEEK_SET) == -1)
		err_sys("lseek error");
	if (write(fd, buf2, 10) != 10)
		err_sys("buf2 write error");
	exit(0);
}

int mainI2()
{
	int n;
	char buf[4096];
	while ((n = read(STDIN_FILENO, buf, 4096)) > 0)
		if (write(STDIN_FILENO, buf, n) != n)
			err_sys("write err");
	if (n < 0)
		err_sys("read error");
	exit(0);
}

int main(int argc, char*argv[])
{
	int val;
	if (argc != 2)
		err_quit("usage: a.out <descriptor#>");
	if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
		err_sys("fcntl error for fd %d", atoi(argv[1]));

	switch (val & O_ACCMODE)
	{
	case O_RDONLY:
		printf("read only");
		break;
	case O_WRONLY:
		printf("write only");
		break;
	case O_RDWR:
		printf("read write");
		break;
	default:
		err_dump("unknow access mode");
	}

	if (val & O_APPEND)
		printf(",append");
	if (val&O_NONBLOCK)
		printf(",nonblocking");
	if (val&O_SYNC)
		printf(",synchronous writes");

#if !defined(_POSIX_C_SOURCE)&&defined(O_FSYNC)&&(O_FSYNC!=O_SYNC)
	if (val &O_FSYNC)
		printf(",synchronous writes");
#endif
	putchar('\n');
	exit(0);
}