#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "apue.h"

#define  DEF_PAGER  "/bin/more"

int main(int argc,char argv[])
{
	int n;
	int fd[2];
	pid_t pid;
	char *pager, *argv0;
	char line[MAXLINE];
	FILE *fp;

	if (argc != 2) {
		err_quit("usage: a.out <pathname>");
	}

	if ((fp = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid > 0) {
		close(fd[0]);
		while (fgets(line, MAXLINE, fp) != NULL) {
			n = strlen(line);
			if (write(fd[1], line, n) != n)
				err_sys("write error to pipe");
		}
		if (ferror(fp))
			err_sys("fgets error");
		close(fd[1]);
		if (waitpid(pid, NULL, 0) < 0)
			err_sys("waitpid error");
		exit(0);
	}
	else {
		close(fd[1]);
		if (fd[0] != STDOUT_FILENO) {
			if (dup2(fd[0], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error to stdin");
			close(fd[0]);  //don't need this after dup2
		}
		if ((pager = getenv("PAGER")) == NULL)
			pager = DEF_PAGER;
		if ((argv0 = strrchr(pager, '/')) != NULL)
			argv0++;
		else
			argv0 = pager;

		if (execl(pager, argv0, (char *)0) < 0)
			err_sys("execl error for %s");
	}

	exit(0);


}


int main01()
{
	int fd[2];
	pid_t pid;

	int ret = pipe(fd);
	if (ret == -1) {
		perror("pipe error");
		return -1;
	}

	pid = fork();
	if (pid == -1) {
		perror("pipe error");
		return -1;
	}
	else if (pid == 0) {  //子 读数据
		//关闭写端
		close(fd[1]);
		char buf[1024];
		ret = read(fd[0], buf, sizeof(buf));
		if (ret == 0) {
			printf("--------\n");
		}
		write(STDOUT_FILENO, buf, ret);
	}
	else {                //父  写
		close(fd[0]);
		char *str = "fsafsdagsagsafd";
		write(fd[1], str, strlen(str));
	}
}
