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
}//*/


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
}//*/

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
}//*/




void main(int arg,char *argv[])
{
	int i=0;
	
	pthread(pthread_test1);
	//fork();
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