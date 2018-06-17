/********************************************
*    file.c 	//add by visual 2016.5.17
*目前是虚拟的文件读写
***********************************************/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


#define	BaseOfEchoFilePhyAddr	(K_PHY2LIN(0x7e00))   //目前就这一个文件
static u32 position=0;

/*****************************************************
*				open 		//add by visual 2016.5.17
*目前没有什么用的open
******************************************************/
//PUBLIC u32 open(char* path,char* mode)
PUBLIC u32 fake_open(char* path,char* mode)		//modified by xw, 18/5/30
{
	position = 0;
	return 0;
}


/******************************************************
*				read		//add by visual 2016.5.17
********************************************************/
//PUBLIC u32 read(u32 fd,void* buffer,u32 size)
PUBLIC u32 fake_read(u32 fd,void* buffer,u32 size)	//modified by xw, 18/5/30
{
	u32 addr_lin = BaseOfEchoFilePhyAddr + position;
	position += size;
	memcpy(buffer,(void*)addr_lin,size);
	return 0;
}

/******************************************************
*				seek //add by visual 2016.5.17
*******************************************************/
//PUBLIC u32 seek(u32 pos)
PUBLIC u32 fake_seek(u32 pos)	//modified by xw, 18/5/30
{
	position = pos;
	return 0;
}

