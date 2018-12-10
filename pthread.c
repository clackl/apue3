#include "apue.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
pthread_t ntid;

void printids(const char *s)
{
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
		(unsigned long)tid, (unsigned long)tid);
}

void* thr_fn(void *arg)
{
	printids("new thread: ");
	return((void*)0);
}

int main01(void)
{
	int err;
	err = pthread_create(&ntid, NULL, thr_fn, NULL);
	if (err != 0)
		err_exit(err, "can't create thread");
	printids("main thread:");
	sleep(1);
	exit(0);
}


void* thr_fn1_1(void *arg)
{
	printf("thread 1 returning\n");
	pthread_exit((void*)1);
}

void* thr_fn2_1(void *arg)
{
	printf("thread 2 exiting\n");
	pthread_exit((void*)2);
}

int main02(void)
{
	int err;
	pthread_t tid1, tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fn1_1, NULL);
	if (err != 0)
		err_exit(err, "can't create thread 1");
	err = pthread_create(&tid2, NULL, thr_fn2_1, NULL);
	if (err != 0)
		err_exit(err, "can't create thread 2");

	err = pthread_join(tid1, &tret);
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	printf("thread 1 exit code %ld\n", (long)tret);
	err = pthread_join(tid2, &tret);
	if (err != 0)
		err_exit(err, "can't join with thread 2");
	printf("thread 2 exit code %ld\n", (long)tret);
	exit(0);
}


void cleanup(void *arg)
{
	printf("cleanup: %s\n", (char*)arg);
}

void *thr_fnl(void *arg)
{
	printf("thread 1 start\n");
	pthread_cleanup_push(cleanup, "thread 1 first handler");
	pthread_cleanup_push(cleanup, "thread 1 second handler");
	printf("thread 1 push complete\n");
	if (arg)
		return((void *)1);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	return ((void*)1);
}

void *thr_fn2(void *arg)
{
	printf("thread 2 start\n");
	pthread_cleanup_push(cleanup, "thread 2 first handler");
	pthread_cleanup_push(cleanup, "thread 2 second handler");
	printf("thread 2 push complete\n");
	if (arg)
		pthread_exit((void *)2);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_exit((void *)2);
}

int main03(void)
{
	int err;
	pthread_t  tid1,tid2;
	void *tret;

	err = pthread_create(&tid1, NULL, thr_fnl, (void*)1);
	if (err != 0)
		err_exit(err, "can't create thread 1");

	err = pthread_create(&tid2, NULL, thr_fn2, (void*)1);
	if (err != 0)
		err_exit(err, "can't create thread 2");

	err = pthread_join(tid1, &tret);
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	printf("thread 1 exit code %ld\n", (long)tret);

	err = pthread_join(tid2, &tret);
	if (err != 0)
		err_exit(err, "can't join with thread 2");
	printf("thread 1 exit code %ld\n", (long)tret);
	exit(0);
}

#define HASH(id) (((unsigned long)id)%29)

struct foo *fh[29];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo
{
	int f_count;
	pthread_mutex_t f_lock;   //protected by hashlock
	int f_id;
	struct foo *f_next;       //protected by hashlock
};

//struct foo* foo_alloc(int id)
//{
//	struct foo *fp;
//	int idx;
//	if ((fp = malloc(sizeof(struct foo))) != NULL) {
//		fp->f_count = 1;
//		fp->f_id = id;
//		if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
//			free(fp);
//			return NULL;
//		}
//		idx = HASH(id);
//		pthread_mutex_lock(&hashlock);
//		fp->f_next = fh[idx];
//		fh[idx] = fp;
//		pthread_mutex_lock(&fp->f_lock);
//		pthread_mutex_unlock(&hashlock);
//		pthread_mutex_unlock(&fp->f_lock);
//	}
//	return fp;
//}
//
//void foo_hold(struct foo *fp)
//{
//	pthread_mutex_lock(&fp->f_lock);
//	fp->f_count++;
//	pthread_mutex_unlock(&fp->f_lock);
//}
//
//struct foo* foo_find(int id)
//{
//	struct foo *fp;
//	pthread_mutex_lock(&hashlock);
//	for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
//		if (fp->f_id == id) {
//			foo_hold(fp);
//			break;
//		}
//	}
//	pthread_mutex_unlock(&hashlock);
//	return(fp);
//}
//
//void foo_rele(struct foo *fp)
//{
//	struct foo *tfp;
//	int idx;
//
//	pthread_mutex_lock(&fp->f_lock);
//	if (fp->f_count == 1) {
//		pthread_mutex_unlock(&fp->f_lock);
//		pthread_mutex_lock(&hashlock);
//		pthread_mutex_destory(&fp->f_lock);
//		if (fp->f_count != 1) {
//			fp->f_count--;
//			pthread_mutex_unlock(&fp->f_lock);
//			pthread_mutex_unlock(&hashlock);
//			return;
//		}
//		idx = HASH(fp->f_id);
//		tfp = fh[idx];
//		if (tfp == fp) {
//			fh[idx] = fp->f_next;
//		}
//		else {
//			while (tfp->f_next != fp)
//				tfp = tfp->f_next;
//			tfp->f_next = fp->f_next;
//		}
//		pthread_mutex_unlock(&hashlock);
//		pthread_mutex_unlock(&fp->f_lock);
//		pthread_mutex_destory(&fp->f_lock);
//
//	}
//	else {
//		fp->f_count--;
//		pthread_mutex_unlock(&fp->f_lock);
//	}
//}



struct foo *foo_alloc(int id)
{
	struct foo *fp;
	int idx;
	if ((fp = malloc(sizeof(struct foo))) != NULL) {
		fp->f_count = 1;
		fp->f_id = id;
		if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
			free(fp);
			return(NULL);
		}
		idx = HASH(id);
		pthread_mutex_lock(&hashlock);
		fp->f_next = fh[idx];
		fh[idx] = fp;
		pthread_mutex_lock(&fp->f_lock);
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_unlock(&fp->f_lock);
	}
	return fp;
}

void foo_hold(struct foo*fp)
{
	pthread_mutex_lock(&hashlock);
	fp->f_count++;
	pthread_mutex_unlock(&hashlock);
}

struct foo* foo_find(int id)  //find an existing object
{
	struct foo *fp;
	pthread_mutex_lock(&hashlock);
	for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
		if (fp->f_id == id) {
			fp->f_count++;
			break;
		}
	}
	pthread_mutex_unlock(&hashlock);
	return fp;
}


void foo_rele(struct foo *fp)  //release a reference to the object
{
	struct foo *tfp;
	int idx;
	pthread_mutex_lock(&hashlock);
	if (--fp->f_count == 0) {  //last reference ,remove from list
		idx = HASH(fp->f_id);
		tfp = fh[idx];
		if (tfp == fp) {
			fh[idx] = fp->f_next;
		}
		else {
			while (tfp->f_next != fp)
				tfp = tfp->f_next;
			tfp->f_next = fp->f_next;
		}
		pthread_mutex_unlock(&hashlock);
		pthread_mutex_destroy(&fp->f_lock);
		free(fp);
	}
	else {
		pthread_mutex_unlock(&hashlock);
	}
}


int main04()
{
	int err;
	struct timespec tout;
	struct tm *tmp;
	char buf[64];
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&lock);
	printf("mutex is locked\n");
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	printf("current time is %s\n", buf);
	tout.tv_sec += 10;
	err = pthread_mutex_timedlock(&lock, &tout);
	clock_gettime(CLOCK_REALTIME,&tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	printf("the time is now %s\n", buf);
	if (err == 0)
		printf("mutex locked again!\n");
	else
		printf("can't lock mvtex again:%s\n", strerror(err));
	exit(0);
}

















