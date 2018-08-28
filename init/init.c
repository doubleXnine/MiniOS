/******************************************************
*	init进程	add by visual 2016.5.16
*******************************************************/

#include "stdio.h"

int global=0;

char *str2,*str3;


void pthread_test3()
{
	int i;
	str2 = malloc(10);
	*(str2+0) = 'M';
	*(str2+1) = 'a';
	*(str2+2) = '\0';
	
	while(1)
	{
		if(str3!=0)
		{
			udisp_str("pth3");
			(*(str2+1)) += 1;
			udisp_str(str3);
			udisp_str(" ");
		}		
		i=10000;
		while(--i){}
	}
}


void pthread_test2()
{
	int i;
	str3 = malloc(10);
	*(str3+0) = 'M';
	*(str3+1) = 'z';
	*(str3+2) = '\0';
	
	pthread(pthread_test3);	
	while(1)
	{
		if(str2!=0)
		{
			udisp_str("pth2");
			(*(str3+1)) -=1;
			udisp_str(str2);
			udisp_str(" ");
		}
		
		i=10000;
		while(--i){}
	}
}

void pthread_test1()
{
	int i;
	pthread(pthread_test2);
	while(1)
	{
		udisp_str("pth1");
		udisp_int(++global);
		udisp_str(" ");
		i=10000;
		while(--i){}
	}
}

/*======================================================================*
                          Syscall Pthread Test
added by xw, 18/4/27
 *======================================================================*/
	/*
int main(int arg,char *argv[])
{
	int i=0;
	
	pthread(pthread_test1);
	while(1)
	{
		udisp_str("init");
		udisp_int(++global);
		udisp_str(" ");
		i=10000;
		while(--i){}
	}
	return 0;
}
//	*/

/*======================================================================*
                          Syscall Fork Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void main(int arg,char *argv[])
{
	int i=0;
	
	fork();
	while(1)
	{
		udisp_str("init");
		udisp_int(++global);
		udisp_str(" ");
		i=10000;
		while(--i){}
	}
	return ;
}
//	*/

/*======================================================================*
                           Syscall Exec Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void main(int arg,char *argv[])
{
	int i=0;
	
	while(1)
	{
		udisp_str("init");
		udisp_int(++global);
		udisp_str(" ");
		i=10000;
		while(--i){}
	}
	return ;
}
//	*/

/*======================================================================*
                           Syscall Yield Test
added by xw, 18/8/16
 *======================================================================*/
	/*
void main(int arg,char *argv[])
{
	int i=0;
	
	while(1)
	{
		udisp_str("U( ");
		yield();
		udisp_str(") ");
		i=10000;
		while(--i){}
	}
	return ;
}
//	*/

/*======================================================================*
                           Syscall Sleep Test
added by xw, 18/8/16
 *======================================================================*/
	/*
void main(int arg,char *argv[])
{
	int i=0;
	
	while(1)
	{
		udisp_str("U( ");
		udisp_str("[");
		udisp_int(get_ticks());
		udisp_str("] ");
		sleep(5);
		udisp_str("[");
		udisp_int(get_ticks());
		udisp_str("] ");
		udisp_str(") ");
		i=10000;
		while(--i){}
	}
	return ;
}
//	*/

/*======================================================================*
                           File System Test
added by xw, 18/6/19
 *======================================================================*/
//	/*
void main(int arg,char *argv[])
{
//	while (1) {}
	
	int fd;
	int i, n;
	const int rd_bytes = 4;
	char filename[MAX_FILENAME_LEN+1] = "blah";
	char bufr[5];
	const char bufw[] = "abcde";

	udisp_str("\n(U)");
	fd = open(filename, O_CREAT | O_RDWR);	
	
	if(fd != -1) {
		udisp_str("File created: ");
		udisp_str(filename);
		udisp_str(" (fd ");
		udisp_int(fd);
		udisp_str(")\n");	
		
		n = write(fd, bufw, strlen(bufw));
		if(n != strlen(bufw)) {
			udisp_str("Write error\n");
		}
		
		close(fd);
	}

	udisp_str("(U)");
	fd = open(filename, O_RDWR);
	udisp_str("   ");
	udisp_str("File opened. fd: ");
	udisp_int(fd);
	udisp_str("\n");

	udisp_str("(U)");
	int lseek_status = lseek(fd, 1, SEEK_SET);
	udisp_str("Return value of lseek is: ");
	udisp_int(lseek_status);
	udisp_str("  \n");

	udisp_str("(U)");
	n = read(fd, bufr, rd_bytes);
	if(n != rd_bytes) {
		udisp_str("Read error\n");
	}
	bufr[n] = 0;
	udisp_str("Bytes read: ");
	udisp_str(bufr);
	udisp_str("\n");

	close(fd);
	
	while (1) {
	}
	
	return;
}
//	*/

