#include "apue.h"
#include <sys/wait.h>

int mains1()
{
	printf("hello world from process ID %ld \n", (long)getpid());
	exit(0);
}

int mains2()
{
	char buf[MAXLINE];
	pid_t pid;
	int status;
	printf("%% ");
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;
		if ((pid = fork()) < 0) {
			err_sys("fork err");
		}
		else if (pid == 0) {
			execlp(buf, buf, (char *)0);
			err_ret("couldn't execute: %s", buf);
			exit(127);
		}

		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid err");
		printf("%%");
	}
	exit(0);
}


static void sig_int(int);
int mains3()
{
	char buf[MAXLINE];
	pid_t pid;
	int status;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error");

	printf("%% ");
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;

		if ((pid = fork()) < 0) {
			err_sys("fork error");
		}
		else if (pid == 0) {
			execlp(buf, buf, (char*)0);
			err_ret("couldn't execute: %s", buf);
			exit(127);
		}

		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid error");
		printf("%% ");
	}
	exit(0);
}

void sig_int(int signo)
{
	printf("interrupt\n%% ");
}