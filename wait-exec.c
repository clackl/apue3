#include<unistd.h>
#include<sys/wait.h>

int main()
{
	int status;
	pid_t pid;
	pid_t wpid;
	int i = 0;
	int n = 3;
	for (i = 0; i < n; i++) {
		if ((pid = fork()) < 0) {
			perror("fork error");
			return -1;
		}
		if (pid == 0) {
			break;
		}
	}
	if (n == i) {
		if (pid > 0) {  /*父进程*/
			sleep(3);
			do {
				wpid = waitpid(-1, &status, WNOHANG);
				if (wpid > 0)
					n--;
				if (WIFEXITED(status))
					printf("Child exited with code %d\n", WEXITSTATUS(status));
				else if(WIFSIGNALED(status))
					printf("Child terminated abnormally, signal %d\n", WTERMSIG(status));
			} while (n > 0);
		}
	}
	if (pid == 0&& i==0) {
		execlp("ls", "ls", "-l", NULL);
	}
	if (pid == 0 && i == 1) {
		execl("/home/clack/server", NULL);
	}
	if (pid == 0 && i == 2) {
		printf("child getpid = %u\n", getpid());
		printf("child getppid = %u\n", getppid());
	}
}

