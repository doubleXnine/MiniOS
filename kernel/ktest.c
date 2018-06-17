/*
 * To test if new kernel features work normally, and if old features still 
 * work normally with new features added.
 * added by xw, 18/4/27
 */
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "fs_const.h"
#include "hd.h"
#include "fs.h"
#include "fs_misc.h"

/*======================================================================*
                         Interrupt Handling Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void TestA()
{
	int i, j;
	while (1)
	{
		disp_str("A ");

		i = 100;
		while(--i){
			j = 1000;
			while(--j){}
		}
	}
}

void TestB()
{
	int i, j;
	while (1)
	{
		disp_str("B ");

		i = 100;
		while(--i){
			j = 1000;
			while(--j){}
		}
	}
}

void TestC()
{
	int i, j;
	while (1)
	{
		disp_str("C ");

		i = 100;
		while(--i){
			j = 1000;
			while(--j){}
		}
	}
}

void initial()
 {
	int i, j;
	while (1)
	{
		disp_str("I ");

		i = 100;
		while(--i){
			j = 1000;
			while(--j){}
		}
	}
 }
//	*/

/*======================================================================*
                        Ordinary System Call Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void TestA()
{
	int i, j;
	while (1)
	{
		disp_str("A ");
		milli_delay(100);
	}
}

void TestB()
{
	int i, j;
	while (1)
	{
		disp_str("B ");
		milli_delay(100);
	}
}

void TestC()
{
	int i, j;
	while (1)
	{
		disp_str("C ");
		milli_delay(100);
	}
}

void initial()
 {
	int i, j;
	while (1)
	{
		disp_str("I ");
		milli_delay(100);
	}
 }
//	*/
	 

/*======================================================================*
                          Kernel Preemption Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void TestA()
{
	int i, j;
	while (1)
	{
		disp_str("A ");
		print_E();
		milli_delay(100);
	}
}

void TestB()
{
	int i, j;
	while (1)
	{
		disp_str("B ");
		print_F();
		milli_delay(100);
	}
}

void TestC()
{
	int i, j;
	while (1)
	{
		disp_str("C ");
		milli_delay(100);
	}
}

void initial()
 {
	int i, j;
	while (1)
	{
		disp_str("I ");
		milli_delay(100);
	}
 }
//	*/

/*======================================================================*
                          Syscall Yield Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void TestA()
{
	int i;
	while (1) 
	{
		disp_str("A( ");
		yield();
		disp_str(") ");
		milli_delay(100);
	} 
}

void TestB()
{
	int i, j;
	while (1)
	{
		disp_str("B ");
		milli_delay(100);
	}
}

void TestC()
{
	int i, j;
	while (1)
	{
		disp_str("C ");
		milli_delay(100);
	}
}

void initial()
 {
	int i, j;
	while (1)
	{
		disp_str("I ");
		milli_delay(100);
	}
 }
//	*/

/*======================================================================*
                          Syscall Sleep Test
added by xw, 18/4/27
 *======================================================================*/
	/*
void TestA()
{
	int i;
	while (1) 
	{
		disp_str("A( ");
		disp_str("[");
		disp_int(ticks);
		disp_str("] ");
		sleep(5);
		disp_str("[");
		disp_int(ticks);
		disp_str("] ");
		disp_str(") ");
		milli_delay(100);
	} 
}

void TestB()
{
	int i, j;
	while (1)
	{
		disp_str("B ");
		milli_delay(100);
	}
}

void TestC()
{
	int i, j;
	while (1)
	{
		disp_str("C ");
		milli_delay(100);
	}
}

void initial()
 {
	int i, j;
	while (1)
	{
		disp_str("I ");
		milli_delay(100);
	}
 }
//	*/

/*======================================================================*
                          Syscall Exec Test
added by xw, 18/4/27
 *======================================================================*/
/* You should also enable Syscall Exec Test in init.c */
//	/*
void TestA()
{
	int i, j;
	while (1)
	{
		disp_str("A ");
		milli_delay(100);
	}
}

void TestB()
{
	int i, j;
	while (1)
	{
		disp_str("B ");
		milli_delay(100);
	}
}

void TestC()
{
	int i, j;
	while (1)
	{
		disp_str("C ");
		milli_delay(100);
	}
}

void initial()
 {
	exec("init/init.bin");
	while(1)
	{
		
	}
 }
//	*/

/*======================================================================*
                          File System Test
added by xw, 18/5/26
 *======================================================================*/
	/*
void TestA()
{	
	int fd;
	int i, n;
	char filename[MAX_FILENAME_LEN+1] = "blah";
	const char bufw[] = "abcde";
	const int rd_bytes = 4;
	char bufr[rd_bytes];
	
	disp_str("In TestA...\n");	// for debug

	fd = open(filename, O_CREAT | O_RDWR);	//create file
	
	if(fd != -1) {
		disp_str("File created: ");
		disp_str(filename);
		disp_str(" (fd ");
		disp_int(fd);
		disp_str(")\n");

		n = write(fd, bufw, strlen(bufw));
		if(n != strlen(bufw)) {
			disp_str("n != strlen(bufw)");
		}

		close(fd);
	}

	char * filenames[] = {"/foo", "/bar", "/baz"};

	// create files
	for (i = 0; i < sizeof(filenames) / sizeof(filenames[0]); i++) {
		fd = open(filenames[i], O_CREAT | O_RDWR);
		if(fd != -1) {
			disp_str("File created: ");
			disp_str(filenames[i]);
			disp_str(" (fd ");
			disp_int(fd);
			disp_str(")\n");

			close(fd);
		}
	}

	char * rfilenames[] = {"/bar", "/foo", "/baz", "/dev_tty0"};

	// remove files
	for (i = 0; i < sizeof(rfilenames) / sizeof(rfilenames[0]); i++) {
		if (unlink(rfilenames[i]) == 0) {
			disp_str("File removed: ");
			disp_str(rfilenames[i]);
			disp_str("\n");
		}
		else {
			disp_str("Failed to remove file: ");
			disp_str(rfilenames[i]);
			disp_str("\n");
		}
	}
	
	while (1) {
	}
}


void TestB()
{
	int fd, n;
	const int rd_bytes = 4;
	char bufr[rd_bytes];
	char filename[MAX_FILENAME_LEN+1] = "blah";

	fd = open(filename, O_RDWR);	//open file

	disp_str("(TestB)File opened. fd: ");
	disp_int(fd);
	disp_str("\n");

	int lseek_status = lseek(fd, 1, SEEK_SET);
	disp_str("(TestB)return value of lseek is: ");
	disp_int(lseek_status);
	disp_str("  ");
	
	n = read(fd, bufr, rd_bytes);
	if(n != rd_bytes) {
		disp_str("n != rd_bytes");
	}
	bufr[n] = 0;
	disp_int(n);
	disp_str(" (TestB)bytes read: ");
	disp_str(bufr);
	disp_str("\n");

	close(fd);
	
	while(1){

	}
}

void TestC()
{
	int fd, n;
	const int rd_bytes = 3;
	char bufr[rd_bytes];
	char filename[MAX_FILENAME_LEN+1] = "blah";
	
	fd = open(filename, O_RDWR);	//open file

	disp_str("(TestC)File opened. fd: ");
	disp_int(fd);
	disp_str("\n");

	int lseek_status = lseek(fd, 1, SEEK_SET);
	disp_str("(TestC)return value of lseek is: ");
	disp_int(lseek_status);
	disp_str("  ");

	n = read(fd, bufr, rd_bytes);
	if(n != rd_bytes) {
		disp_str("n != rd_bytes");
	}
	bufr[n] = 0;
	disp_int(n);
	disp_str(" (TestC)bytes read: ");
	disp_str(bufr);
	disp_str("\n");

	close(fd);
	
	while(1){
	}
}

void initial()
{
	while (1)
	{
		// disp_str("I ");
		// milli_delay(100);
	}
}
//	*/





