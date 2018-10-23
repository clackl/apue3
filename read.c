#include "apue.h"


int mainr1()
{
	int n;
	char buf[4096];
	while ((n = read(STDIN_FILENO, buf, 4096)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");
	if (n < 0)
		err_sys("read error");
	exit(0);
}
//STDIN_FILENO：接收键盘的输入
//STDOUT_FILENO：向屏幕输出
//  如果执行./a.out > data  将会输出到data文件中而不会在屏幕输出显示
//  如果执行./a.out < infile > outfile  将会将inflie中的内容复制到outfile中



int mainr2()
{
	int c;
	while ((c = getc(stdin)) != EOF)
		if (putc(c, stdout) == EOF)
			err_sys("output error");
	if (ferror(stdin))
		err_sys("input error");
	exit(0);
}