#include "apue.h"

int main01()
{
	if (chdir("/tmp") < 0)
		err_sys("chdir failed");
	printf("chdir to /tmp succeeded\n");
	exit(0);
}

int main()
{
	char *ptr;
	size_t size;
	if (chdir("/home/clack") < 0)
		err_sys("chdir failed");
	ptr = path_alloc(&size);  
	if (getcwd(ptr, size) == NULL)
		err_sys("getcwd filed");
	printf("cwd = %s\n", ptr);
	exit(0);
}