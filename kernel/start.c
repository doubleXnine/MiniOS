
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            start.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                            cstart
 *======================================================================*/
PUBLIC void cstart()
{
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n-----\"cstart\" begins-----\n");

	// 将 LOADER 中的 GDT 复制到新的 GDT 中
	memcpy(	&gdt,				    // New GDT
		(void*)(*((u32*)(&gdt_ptr[2]))),   // Base  of Old GDT
		*((u16*)(&gdt_ptr[0])) + 1	    // Limit of Old GDT
		);
	// gdt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sgdt 以及 lgdt 的参数。
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base  = (u32)&gdt;

	// idt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sidt 以及 lidt 的参数。
	u16* p_idt_limit = (u16*)(&idt_ptr[0]);
	u32* p_idt_base  = (u32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
	*p_idt_base  = (u32)&idt;

	init_prot();

	disp_str("-----\"cstart\" finished-----\n");
}

/*======================================================================*
                           init_page_tbl		delete by visual 2016.4.19
 *======================================================================
PUBLIC	void init_page_tbl()
{
	PageTblNum = *(u32*)PageTblNumAddr;		//读取保存的内存信息
	//disp_int(PageTblNum);	
	
	int i,j;
	int *addr_temp;	
	int	*value_temp;
	int temp;
	
	for(i=0;  i<NR_PCBS ; i++) 									
	{//NR_PCBS是进程表的数量,每个进程一个页目录
		addr_temp =	(u32*)PageDirBaseProc + 1024*(PageTblNum+1) * i;
		value_temp = addr_temp + 1024;
		for(j=0; j<PageTblNum; j++)
		{//页目录
			*(addr_temp) = (((u32)value_temp) & 0xFFFFF000) | PG_P  | PG_USU | PG_RWW;	//1111 1111 1111 1111 0000 0000 0000
			addr_temp++;
			value_temp += 1024;
		}
		
		addr_temp = (u32*)PageDirBaseProc + 1024*(PageTblNum+1) * i + 1024;//跳过页目录
		value_temp = 0; //用这个变量,表示物理地址,从0开始
		for(j=1024; j<2048 ; j++)
		{//第一页,与前4M物理地址一一对应
			*(addr_temp) = (((u32)value_temp) & 0xFFFFF000) | PG_P  | PG_USU | PG_RWW;  
			addr_temp++;
			value_temp += 1024;
		}
		
		temp = 1024 *(PageTblNum+1);
		for(  ; j<temp ; j++)
		{//其它页也与物理地址一一对应
			*(addr_temp) = (((u32)value_temp) & 0xFFFFF000) | PG_P  | PG_USU | PG_RWW;  //1111 1111 1111 1111 0000 0000 0000 
			addr_temp++;
			value_temp += 1024;  
		}
	}
}*/
