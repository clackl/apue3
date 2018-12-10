#include "apue.h"
#include <pwd.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static void sig_usr(int);

int main01(void)
{
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("can't catch SIGUSR1");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("can't catch SIGUSR2");
	for (;;)
		pause();
}
static void sig_usr(int signo)
{
	if (signo == SIGUSR1)
		printf("received SIGUSR1\n");
	else if (signo == SIGUSR2)
		printf("received SIGUSR2\n");
	else
		err_dump("received signal %d\n", signo);
}

static void my_alarm(int signo)
{
	struct passwd *rootptr;
	printf("in signal handler\n");
	if ((rootptr = getpwnam("clack")) == NULL)
		err_sys("getpwnam(clack) error");
	alarm(1);
}

int main02()
{
	struct passwd *ptr;
	signal(SIGALRM, my_alarm);
	alarm(1);
	for (;;) {
		if ((ptr = getpwnam("clack")) == NULL)
			err_sys("getpwnam error");
		if (strcmp(ptr->pw_name, "clack") != 0)
			printf("return value corrupted!,pw_name = %s\n", ptr->pw_name);
	}
}


static void sig_cld(int);

int main03()
{
	pid_t pid;
	if (signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal error");
	if ((pid = fork()) < 0) {
		perror("fork error");
	}
	else if (pid == 0) {
		sleep(2);
		_exit(0);
	}
	pause();
	exit(0);
}

static void sig_cld(int signo)
{
	pid_t pid;
	int status;
	printf("SIGCLD receive\n");

	if (signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal error");
}


static void sig_alrm(int signo)
{

}

unsigned int sleepl(unsigned int seconds)
{
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		return(seconds);
	alarm(seconds);   //start the timer
	pause();          //next caught signal wakes us up
	return(alarm(0)); //turn off timer,return unslept time
}


static jmp_buf env_alrm;

static void sig_alrm1(int signo)
{
	longjmp(env_alrm, 1);
}

unsigned int sleep2(unsigned int seconds)
{
	if (signal(SIGALRM, sig_alrm1) == SIG_ERR)
		return(seconds);
	if (setjmp(env_alrm) == 0) {
		alarm(seconds);
		pause();
	}
	return (alarm(0));
}

static void sig_int(int);

int main04()
{
	unsigned int unslept;
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	unslept = sleep2(5);
	printf("sleep2 returned: %u\n", unslept);
	exit(0);
}

static void sig_int(int signo)
{
	int i, j;
	volatile int k;
	printf("\nsig_int starting\n");
	for (i = 0; i < 300000; i++) 
		for (j = 0; j < 4000; j++)
			k += i * j;
	printf("sig_int finished\n");
}



static void sig_quit(int);

int main05(void)
{
	sigset_t newmask, oldmask, pendmask;
	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("can't catch SIGQUIT");

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
	sleep(5);

	if (sigpending(&pendmask) < 0)
		err_sys("sigpending error");
	if (sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
	printf("SIGQUIT unblocked\n");

	sleep(5);
	exit(0);
}

static void sig_quit(int signo)
{
	printf("caught SIGQUIT\n");
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
}














