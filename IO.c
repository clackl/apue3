#include "apue.h"
#include <fcntl.h>
#include <errno.h>

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

int mainI3()
{
	int c;
	while ((c = getc(stdin)) != EOF)
		if (putc(c, stdout) == EOF)
			err_sys("output error");
	if (ferror(stdin))
		err_sys("input error");
	exit(0);
}


int mainI4()
{
	char buf[MAXLINE];
	while (fgets(buf, MAXLINE, stdin) != NULL)
		if(fputs(buf,stdout) == EOF)
			err_sys("output error");
	if (ferror(stdin))
		err_sys("input error");
	exit(0);
}

void pr_stdio(const char *, FILE*);
int is_unbufferer(FILE*);
int is_linebuffered(FILE*);
int buffer_size(FILE*);

int mainI5()
{
	FILE *fp;
	fputs("enter any characte\n", stdout);
	if (getchar() == EOF)
		err_sys("getchar error");
	fputs("one line to standrd error\n", stderr);

	pr_stdio("stdin", stdin);
	pr_stdio("stdout", stdout);
	pr_stdio("stderr", stderr);

	if ((fp = fopen("/etc/passwd", "r")) == NULL)
		err_sys("fopen error");
	if (getc(fp) == EOF)
		err_sys("getc error");
	pr_stdio("/etc/passwd", fp);
	exit(0);
}

void pr_stdio(const char *name, FILE *fp)
{
	printf("stream = %s,", name);
	if (is_unbuffered(fp))
		printf("unbuffered");
	else if (is_linebuffered(fp))
		printf("line buffered");
	else
		printf("fully buiffered");
	printf(",buffer size = %d\n", buffer_size(fp));
}

#if defined(_IO_UNBUFFERED)

int is_unbuffered(FILE *fp) {
	return (fp->_flags&_IO_UNBUFFERED);
}

int is_linebuffered(FILE *fp) {
	return (fp->_flags&_IO_LINE_BUF);
}

int buffer_size(FILE *fp) {
	return (fp->_IO_buf_end - fp->_IO_buf_base);
}

#endif


int mainI6()
{
	char name[L_tmpnam], line[MAXLINE];
	FILE *fp;
	printf("%s\n", tmpnam(NULL));   //first temp name

	tmpnam(name);     //second temp name
	printf("%s\n", name);

	if ((fp = tmpfile()) == NULL)   //create temp file
		err_sys("tmpfile error");
	fputs("one line of output\n", fp);  //write to temp file
	rewind(fp);             //then read it back
	if (fgets(line, sizeof(line), fp) == NULL)
		err_sys("fgets error");
	fputs(line, stdout);      //print the line we wrote
	exit(0);
}

void make_temp(char *template);

int mainI7()
{
	char good_template[] = "/tmp/dirXXXXXX";   //right way
	char *bad_template = "/tmp/dirXXXXXX";   //wrong way

	printf("tring to create first temp file...\n");
	make_temp(good_template);
	printf("tring to create second temp file...\n");
	make_temp(bad_template);
	exit(0);
}

void make_temp(char *template)
{
	int fd;
	struct stat sbuf;
	if ((fd = mkstemp(template)) < 0)
		err_sys("can't create temp file");
	printf("temp name = %s\n", template);
	close(fd);
	if (stat(template, &sbuf) < 0) {
		if (errno == ENOENT)
			printf("file doesn't exist\n");
		else
			err_sys("stat failed");
	}
	else {
		printf("file exist\n");
		unlink(template);
	}
}



#define BSZ 48

int main()
{
	FILE *fp;
	char buf[BSZ];
	memset(buf, 'a', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
		err_sys("fmemopen failed");
	printf("initial buffer contents: %s\n", buf);
	fprintf(fp, "hello,world");
	printf("before flush:%s\n", buf);
	fflush(fp);
	printf("after fflush: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'b', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	fprintf(fp, "hello,world");
	fseek(fp, 0, SEEK_SET);
	printf("after fseek: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'c', BSZ - 2);
	buf[BSZ - 2] = '\0';
	buf[BSZ - 1] = 'X';
	fprintf(fp, "hello,world");
	fclose(fp);
	printf("after fclose: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));
}













