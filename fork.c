#include "apue.h"
#include "pr_exit.h"
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
int globvar = 6;    //external variable in initalized data
char buf[] = "a write to stdout\n";

int main_01(void)
{
	int var;
	pid_t pid;

	var = 88;
	if (write(STDOUT_FILENO, buf, sizeof(buf) - 1) != sizeof(buf) - 1)
		err_sys("write error");
	printf("before fork\n");

	if ((pid = fork()) < 0) {
		err_sys("fork err");
	}
	else if (pid == 0) {
		globvar++;
		var++;
	}
	else {
		sleep(2);
	}

	printf("pid = %ld,glob = %d,var = %d\n", (long)getpid(), globvar, var);
	exit(0);
}



int main_02(void)
{
	pid_t pid;
	int status;

	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)
		exit(7);

	if (wait(&status) != pid)
		err_sys("wait error");
	pr_exit(status);

	if ((pid = fork()) < 0)
		err_sys("forl error");
	else if (pid == 0)
		abort();                //SIGABRT

	if (wait(&status) != pid)
		err_sys("wait error");
	pr_exit(status);

	if ((pid = fork()) < 0)
		err_sys("forl error");
	else if (pid == 0)
		status /= 0;                //SIGFPE

	if (wait(&status) != pid)
		err_sys("wait error");
	pr_exit(status);
}


//如果一个进程fork一个子进程，但不要他等待子进程终止，也不希望子进程处于僵死转态直到父进程终止
int main_03()
{
	pid_t pid;
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid == 0) {
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid > 0)
			exit(0);     //parent from second fork == first child

		sleep(2);
		printf("second child ,parent pid = %ld\n", (long)getppid());
		exit(0);
	}

	if (waitpid(pid, NULL, 0) != pid)  //wait for first child
		err_sys("waitpid error");

	exit(0);
}


static void charatatime(char *);

int main_04()
{
	pid_t pid;
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid == 0) {
		charatatime("output from child\n");
	}
	else {
		charatatime("output from parent\n");
	}
	exit(0);
}

static void charatatime(char *str)
{
	char *ptr;
	int c;
	setbuf(stdout, NULL);     //set unbuffered
	for (ptr = str; (c = *ptr++) != 0;)
		putc(c, stdout);
}


int main_05()
{
	pid_t pid;
	TELL_WAIT();
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid == 0) {
		WAIT_PARENT();
		charatatime("output from child\n");
	}
	else {
		charatatime("output from parent\n");
		TELL_CHILD(pid);
	}
	exit(0);
}


char *env_init[] = { "USEr = unknown","PATH = /tmp",NULL };
int main_06()
{
	pid_t pid;
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid == 0) {
		if (execle("/home/clack/projects/apue3/echoall", "echoall", "myargl", "MY ARG2", (char *)0, env_init) < 0)
			err_sys("elecle error");
	}
	if (waitpid(pid, NULL, 0)) {
		err_sys("wait error");
	}

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid == 0) {
		if (execlp("echoall", "echoall", "only 1 arg", (char *)0) < 0)
			err_sys("execlp error");
	}
	exit(0);
}

int main_07(int argc,char *argv[])
{
	int i;
	char **ptr;
	extern char **environ;
	for (i = 0; i < argc; i++)
		printf("argv[%d]: %s\n", i, argv[i]);
	for (ptr = environ; ptr != 0; ptr++)
		printf("%s\n", *ptr);
	exit(0);
}

int main_08(void)
{
	pid_t pid;
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid == 0) {
		if (execl("/home/clack/projects/apue3/echoall", "echoall", "myarg1", "MY ARG2", (char *)0) < 0)
			err_sys("execl error");
	}
	if (waitpid(pid, NULL, 0) < 0)
		err_sys("waitpid error");
	exit(0);
}

int systeam(const char *cmdstring)
{
	pid_t pid;
	int status;

	if (cmdstring == NULL)
		return (1);
	if ((pid = fork()) < 0) {
		status = -1;
	}
	else if (pid == 0) {
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);
	}
	else {
		while (waitpid(pid, &status, 0) < 0) {
			if (errno != EINTR) {
				status = -1;    //error other than EINTR from waitpid
				break;
			}
		}
	}
	return(status);
}


int main_09(void)
{
	int status;
	if ((status = systeam("date")) < 0)
		err_sys("systeam() error");
	pr_exit(status);

	if ((status = systeam("nosuchcommand")) < 0)
		err_sys("system() error");
	pr_exit(status);

	if ((status = systeam("who; exit 44")) < 0)
		err_sys("system() error");
	pr_exit(status);
	exit(0);
}


static void sig_hup(int signo) 
{
	printf("SIGHUP recevid,pid = %ld\n", (long)getpid());
}

static void pr_ids(char *name)
{
	printf("%s: pid = %ld,ppid = %ld,pgrp = %ld,tpgrp = %ld\n",
		name, (long)getpid(), (long)getppid(), (long)getpgrp(),
		(long)tcgetpgrp(STDIN_FILENO));
	fflush(stdout);
}

int main_10()
{
	char c;
	pid_t pid;
	pr_ids("parent");

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	}
	else if (pid > 0) {
		sleep(5);
	}
	else {
		pr_ids("child");
		signal(SIGHUP, sig_hup);
		kill(getpid(), SIGTSTP);
		pr_ids("child");
		if (read(STDIN_FILENO, &c, 1) != 1)
			printf("read error %d on controlling TTY\n", errno);
	}
	exit(0);
}

