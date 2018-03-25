/*************************************************************
*页式管理相关代码 add by visual 2016.4.19
**************************************************************/


#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                           switch_pde
 *switch the page directory table after schedule() is called
 *======================================================================*/
PUBLIC	void switch_pde()
{
	cr3_ready = p_proc_current->task.cr3;
}

/*======================================================================*
                           init_page_pte		add by visual 2016.4.19
*该函数只初始化了进程的高端（内核端）地址页表
 *======================================================================*/
PUBLIC	u32 init_page_pte(u32 pid)
{//页表初始化函数
	
	u32 AddrLin,pde_addr_phy_temp,pte_addr_phy_temp,err_temp;
	
	pde_addr_phy_temp = test_kmalloc_4k();//为页目录申请一页
	memset((void*)K_PHY2LIN(pde_addr_phy_temp),0,num_4K);   //add by visual 2016.5.26
	
	if( pde_addr_phy_temp<0 || (pde_addr_phy_temp&0x3FF)!=0 ) 	//add by visual 2016.5.9
	{	
		disp_color_str("init_page_pte Error:pde_addr_phy_temp",0x74);
		return -1;
	}

	proc_table[pid].task.cr3 = pde_addr_phy_temp;//初始化了进程表中cr3寄存器变量，属性位暂时不管
	/*********************页表初始化部分*********************************/
	u32 phy_addr=0;
	
	for( AddrLin=KernelLinBase,phy_addr=0; AddrLin<KernelLinBase+KernelSize; AddrLin+=num_4K,phy_addr+=num_4K )
	{//只初始化内核部分，3G后的线性地址映射到物理地址开始处	
		err_temp = lin_mapping_phy(	AddrLin,//线性地址					//add by visual 2016.5.9
									phy_addr,//物理地址	
									pid,//进程pid						//edit by visual 2016.5.19
									PG_P  | PG_USU | PG_RWW,//页目录的属性位（用户权限）			//edit by visual 2016.5.26 
									PG_P  | PG_USS | PG_RWW);//页表的属性位（系统权限）				//edit by visual 2016.5.17 
		if( err_temp!=0 )
		{
			disp_color_str("init_page_pte Error:lin_mapping_phy",0x74);
			return -1;
		}
	}
	
	return 0;
}


/*======================================================================*
                           page_fault_handle		edit by visual 2016.5.9
 *======================================================================*/
PUBLIC void page_fault_handler(	u32 vec_no,//异常编号，此时应该是14，代表缺页异常
								u32 err_code,//错误码
								u32 eip,//导致缺页的指令的线性地址
								u32 cs,//发生错误时的代码段寄存器内容 
								u32 eflags)//时发生错误的标志寄存器内容
{//缺页中断处理函数
	/*	打印出灰底红字的[Page Fault!]
	*	修正该页错误
	*	打印出灰底红字的[Solved]
	*/
	//edit by visual 2016.4.28
	u32 cr2 = read_cr2();				//add by visual 2016.5.9
	u32 pde_addr_phy_temp = get_pde_phy_addr(p_proc_current->task.pid);//获取该进程页目录物理地址
	u32 pte_addr_phy_temp = get_pte_phy_addr(p_proc_current->task.pid,cr2);//获取该线性地址对应的页表的物理地址//edit by visual 2016.5.19
	
	disp_color_str("PAGE FAULT!",0x74);
	disp_color_str("Cr2=",0x74);	//灰底红字 
	disp_int(cr2);
	disp_color_str("eip=",0x74);	//灰底红字 
	disp_int(eip);
	disp_color_str("eflags=",0x74);
	disp_int(eflags);
	disp_color_str("cs=",0x74);
	disp_int(cs);
	disp_color_str("err_code=",0x74);
	disp_int(err_code);
	disp_color_str("Cr3=",0x74);
	disp_int(p_proc_current->task.cr3);//获取页目录中填写的内容
	disp_color_str("Dir=",0x74);
	disp_int(*((u32*)K_PHY2LIN(pde_addr_phy_temp) + get_pde_index(cr2)));//获取页目录中填写的内容
	disp_color_str("Tbl=",0x74);
	disp_int(*((u32*)K_PHY2LIN(pte_addr_phy_temp) + get_pte_index(cr2)));//获取页表中填写的内容
	if( 0==pte_exist(pde_addr_phy_temp,cr2))
	{//页表不存在
		disp_color_str("[Tbl Fault!]",0x74);	//灰底红字 	
		(*((u32*)K_PHY2LIN(pde_addr_phy_temp) + get_pde_index(cr2))) |= PG_P;
		disp_color_str("[Solved]",0x74);
	}
	else
	{//只是缺少物理页   
		disp_color_str("[Page Fault!]",0x74);	//灰底红字		
		(*((u32*)K_PHY2LIN(pte_addr_phy_temp) + get_pte_index(cr2)))|= PG_P;		 
		disp_color_str("[Solved]",0x74);
	}
	refresh_page_cache();
}
 
/***************************地址转换过程***************************
*
*第一步，CR3包含着页目录的起始地址，用32位线性地址的最高10位A31~A22作为页目录的页目录项的索引，
*将它乘以4，与CR3中的页目录的起始地址相加，形成相应页表的地址。
*
*第二步，从指定的地址中取出32位页目录项，它的低12位为0，这32位是页表的起始地址。
*用32位线性地址中的A21~A12位作为页表中的页面的索引，将它乘以4，与页表的起始地址相加，形成32位页面地址。
* 
*第三步，将A11~A0作为相对于页面地址的偏移量，与32位页面地址相加，形成32位物理地址。
*************************************************************************/
 
/*======================================================================*
                          get_pde_index		add by visual 2016.4.28
 *======================================================================*/
PUBLIC	inline u32 get_pde_index(u32 AddrLin)
{//由 线性地址 得到 页目录项编号
	return (AddrLin>>22);//高10位A31~A22
}



/*======================================================================*
                          get_pte_index		add by visual 2016.4.28
 *======================================================================*/
PUBLIC inline u32 get_pte_index(u32 AddrLin)
{//由 线性地址 得到 页表项编号   
	return (((AddrLin)&0x003FFFFF)>>12);//中间10位A21~A12,0x3FFFFF = 0000 0000 0011 1111 1111 1111 1111 1111
}
 
 
 
 /*======================================================================*
                          get_pde_phy_addr	add by visual 2016.4.28
 *======================================================================*/
 PUBLIC inline u32 get_pde_phy_addr(u32 pid)
 {//获取页目录物理地址
	 if( proc_table[pid].task.cr3==0 )
	 {//还没有初始化页目录
		return -1;
	 }
	 else
	 {
		return ((proc_table[pid].task.cr3)&0xFFFFF000);
	 }	
 }
 
 
 /*======================================================================*
                          get_pte_phy_addr	add by visual 2016.4.28
 *======================================================================*/
PUBLIC inline u32 get_pte_phy_addr(	u32 pid,//页目录物理地址		//edit by visual 2016.5.19
								u32 AddrLin)//线性地址
{//获取该线性地址所属页表的物理地址
	u32 PageDirPhyAddr = get_pde_phy_addr(pid);				//add by visual 2016.5.19
	return (*((u32*)K_PHY2LIN(PageDirPhyAddr) + get_pde_index(AddrLin)))&0xFFFFF000;//先找到该进程页目录首地址，然后计算出该线性地址对应的页目录项，再访问,最后注意4k对齐
}
 
 /*======================================================================*
                          get_page_phy_addr	add by visual 2016.5.9
 *======================================================================*/
PUBLIC inline u32 get_page_phy_addr(	u32 pid,//页表物理地址				//edit by visual 2016.5.19
								u32 AddrLin)//线性地址
{//获取该线性地址对应的物理页物理地址  
	u32 PageTblPhyAddr = get_pte_phy_addr(pid,AddrLin);			//add by visual 2016.5.19
	return (*((u32*)K_PHY2LIN(PageTblPhyAddr) + get_pte_index(AddrLin)))&0xFFFFF000;  		
}
 
 /*======================================================================*
                          pte_exist		add by visual 2016.4.28
 *======================================================================*/
PUBLIC u32 pte_exist(	u32 PageDirPhyAddr,//页目录物理地址
						u32 AddrLin)//线性地址
{//判断 有没有 页表
	if( (0x00000001&(*((u32*)K_PHY2LIN(PageDirPhyAddr) + get_pde_index(AddrLin))))==0 )  //先找到该进程页目录,然后计算出该线性地址对应的页目录项,访问并判断其是否存在
	{//标志位为0，不存在
		return 0;
	}
	else 
	{
		return 1;
	}
}
 
 
  /*======================================================================*
                          phy_exist		add by visual 2016.4.28
 *======================================================================*/
PUBLIC u32 phy_exist(u32 PageTblPhyAddr,//页表物理地址
					u32 AddrLin)//线性地址
{//判断 该线性地址 有没有 对应的 物理页
	if( (0x00000001&(*((u32*)K_PHY2LIN(PageTblPhyAddr) + get_pte_index(AddrLin))))==0 )  
	{//标志位为0，不存在
		return 0;
	}
	else 
	{
		return 1;
	}			
}
 
 /*======================================================================*
                          write_page_pde		add by visual 2016.4.28
 *======================================================================*/
PUBLIC void write_page_pde(	u32 PageDirPhyAddr,//页目录物理地址
							u32	AddrLin,//线性地址
							u32 TblPhyAddr,//要填写的页表的物理地址（函数会进行4k对齐）
							u32 Attribute)//属性
{//填写页目录
	(*((u32*)K_PHY2LIN(PageDirPhyAddr) + get_pde_index(AddrLin))) = (TblPhyAddr&0xFFFFF000) | Attribute;
	//进程页目录起始地址+每一项的大小*所属的项
}



/*======================================================================*
                          write_page_pte		add by visual 2016.4.28
 *======================================================================*/
PUBLIC void write_page_pte(	u32 TblPhyAddr,//页表物理地址
							u32	AddrLin,//线性地址
							u32 PhyAddr,//要填写的物理页物理地址(任意的物理地址，函数会进行4k对齐)
							u32 Attribute)//属性
{//填写页目录，会添加属性
	(*((u32*)K_PHY2LIN(TblPhyAddr) + get_pte_index(AddrLin))) = (PhyAddr&0xFFFFF000) | Attribute;
	//页表起始地址+一项的大小*所属的项
}


/*======================================================================*
*                         vmalloc		add by visual 2016.5.4
*从堆中分配size大小的内存，返回线性地址
*======================================================================*/
PUBLIC u32 vmalloc(	u32 size)
{
	u32 temp;
	if(p_proc_current->task.info.type == TYPE_PROCESS )
	{//进程直接就是标识
		temp= p_proc_current->task.memmap.heap_lin_limit;
		p_proc_current->task.memmap.heap_lin_limit += size;
	}
	else
	{//线程需要取父进程的标识
		temp= *((u32*)p_proc_current->task.memmap.heap_lin_limit);
		(*((u32*)p_proc_current->task.memmap.heap_lin_limit)) += size;
	}
	
	return temp;
}

/*======================================================================*
*                          lin_mapping_phy		add by visual 2016.5.9
*将线性地址映射到物理地址上去,函数内部会分配物理地址
*======================================================================*/
PUBLIC int lin_mapping_phy(u32 AddrLin,//线性地址
						u32 phy_addr,//物理地址,若为MAX_UNSIGNED_INT(0xFFFFFFFF)，则表示需要由该函数判断是否分配物理地址，否则将phy_addr直接和AddrLin建立映射
						u32 pid,//进程pid						//edit by visual 2016.5.19
						u32 pde_Attribute,//页目录中的属性位
						u32 pte_Attribute)//页表中的属性位
{
	u32 pte_addr_phy;
	u32 pde_addr_phy = get_pde_phy_addr(pid);						//add by visual 2016.5.19
	
	if( 0==pte_exist(pde_addr_phy,AddrLin) )
	{//页表不存在，创建一个，并填进页目录中
		pte_addr_phy = (u32)test_kmalloc_4k(); //为页表申请一页
		memset((void*)K_PHY2LIN(pte_addr_phy),0,num_4K);		//add by visual 2016.5.26
		
		if( pte_addr_phy<0 || (pte_addr_phy&0x3FF)!=0 ) 	//add by visual 2016.5.9
		{	
			disp_color_str("lin_mapping_phy Error:pte_addr_phy",0x74);
			return -1;
		}
				
		write_page_pde(	pde_addr_phy,//页目录物理地址
						AddrLin,//线性地址
						pte_addr_phy,//页表物理地址
						pde_Attribute);//属性	
	}
	else
	{//页表存在，获取该页表物理地址
		pte_addr_phy = get_pte_phy_addr(	pid,//进程pid			//edit by visual 2016.5.19
											AddrLin);//线性地址
	}
	
	if( MAX_UNSIGNED_INT==phy_addr )			//add by visual 2016.5.19
	{//由函数申请内存
		if( 0==phy_exist(pte_addr_phy,AddrLin) ) 
		{//无物理页，申请物理页并修改phy_addr
			if( AddrLin>=K_PHY2LIN(0) ) phy_addr = test_kmalloc_4k();//从内核物理地址申请一页	
			else 
			{
				//disp_str("%");
				phy_addr = test_malloc_4k();//从用户物理地址空间申请一页
			}
		}
		else
		{
			//有物理页，什么也不做,直接返回，必须返回
			return 0;
		}
	}
	else
	{//指定填写phy_addr
		//不用修改phy_addr
	}
	
	if( phy_addr<0 || (phy_addr&0x3FF)!=0  )
	{
		disp_color_str("lin_mapping_phy:phy_addr ERROR",0x74);
		return -1;
	}
	
	write_page_pte(	pte_addr_phy,//页表物理地址
					AddrLin,//线性地址
					phy_addr,//物理页物理地址
					pte_Attribute);//属性
	refresh_page_cache();
	
	return 0;
}
	

/*======================================================================*
*                          clear_kernel_pagepte_low		add by visual 2016.5.12
*将内核低端页表清除
*======================================================================*/	
void clear_kernel_pagepte_low()
{
	u32 page_num = *(u32*)PageTblNumAddr; 
	memset((void*)(K_PHY2LIN(KernelPageTblAddr)),0,4*page_num);		//从内核页目录中清除内核页目录项前8项		
	memset((void*)(K_PHY2LIN(KernelPageTblAddr+0x1000)),0,4096*page_num);	//从内核页表中清除线性地址的低端映射关系
	refresh_page_cache();
}
