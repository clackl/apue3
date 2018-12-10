#include "apue.h"
#include <fcntl.h>

#define mode S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH

int main()
{
	umask(0);
	if (creat("foo", mode) < 0)
		err_sys("creat error for foo");
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (creat("bar", mode) < 0)
		err_sys("creat error for bar");
	exit(0);
}