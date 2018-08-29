/*********************************************************
*系统调用具体函数的实现
*
*
*
**********************************************************/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

struct memfree *memarg = 0;

/*======================================================================*
                           sys_get_ticks		add by visual 2016.4.6
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}


/*======================================================================*
                           sys_get_pid		add by visual 2016.4.6
 *======================================================================*/
PUBLIC int sys_get_pid()
{
	return p_proc_current->task.pid;
}

/*======================================================================*
                           sys_kmalloc		add by visual 2016.4.6
 *======================================================================*/
PUBLIC void* sys_kmalloc(int size)
{						//edit by visual 2015.5.9
	return (void*)(test_kmalloc(size));
}

/*======================================================================*
                           sys_kmalloc_4k		add by visual 2016.4.7
 *======================================================================*/
PUBLIC void* sys_kmalloc_4k()
{						
	return (void*)(test_kmalloc_4k());
}
 
/*======================================================================*
                           sys_malloc		edit by visual 2016.5.4
 *======================================================================*/
PUBLIC void* sys_malloc(int size)		
{	
	int vir_addr,phy_addr,AddrLin,pde_addr_phy,pte_addr_phy;
	vir_addr = vmalloc(size);
	
	for( AddrLin=vir_addr; AddrLin<vir_addr+size ; AddrLin += num_4B )//一个字节一个字节处理
	{
		lin_mapping_phy(	AddrLin,//线性地址					//add by visual 2016.5.9
							MAX_UNSIGNED_INT,//物理地址						//edit by visual 2016.5.19
							p_proc_current->task.pid,//进程pid				//edit by visual 2016.5.19
							PG_P  | PG_USU | PG_RWW,//页目录的属性位
							PG_P  | PG_USU | PG_RWW);//页表的属性位		
	}
	return (void*)vir_addr;
}


/*======================================================================*
                           sys_malloc_4k		edit by visual 2016.5.4
 *======================================================================*/
PUBLIC void* sys_malloc_4k()
{	
	int vir_addr,AddrLin,pde_addr_phy,pte_addr_phy,phy_addr;
	vir_addr = vmalloc(num_4K);
	
	for( AddrLin=vir_addr; AddrLin<vir_addr+num_4K ; AddrLin += num_4K )//一页一页处理(事实上只有一页，而且一定没有填写页表，页目录是否填写不确定)
	{
		lin_mapping_phy(	AddrLin,//线性地址					//add by visual 2016.5.9
							MAX_UNSIGNED_INT,//物理地址	
							p_proc_current->task.pid,//进程pid					//edit by visual 2016.5.19
							PG_P  | PG_USU | PG_RWW,//页目录的属性位
							PG_P  | PG_USU | PG_RWW);//页表的属性位				
	}
	return (void*)vir_addr;
}


/*======================================================================*
                           sys_free		add by visual 2016.4.7
 *======================================================================*/
PUBLIC int sys_free(void *arg)
{	
	memarg = (struct memfree *)arg;
	return test_free(memarg->addr,memarg->size);
}

/*======================================================================*
                           sys_free_4k		edit by visual 2016.5.9
 *======================================================================*/
PUBLIC int sys_free_4k(void* AddrLin)
{//线性地址可以不释放，但是页表映射关系必须清除！
	int phy_addr;				//add by visual 2016.5.9
	
	phy_addr = get_page_phy_addr(p_proc_current->task.pid,(int)AddrLin);//获取物理页的物理地址		//edit by visual 2016.5.19
	lin_mapping_phy(	(int)AddrLin,//线性地址					
						phy_addr,//物理地址
						p_proc_current->task.pid,//进程pid			//edit by visual 2016.5.19
						PG_P  | PG_USU | PG_RWW,//页目录的属性位
						0  | PG_USU | PG_RWW);//页表的属性位	
	return test_free_4k(phy_addr);
}

/*======================================================================*
*                          sys_fork		add by visual 2016.4.8
*sys_fork()放在了fork.c文件中						    
*======================================================================*/


/*======================================================================*
                           sys_pthread		add by visual 2016.4.11
*sys_pthread()放在了pthread.c文件中		
 *======================================================================*/


/*======================================================================*
                           sys_udisp_int		add by visual 2016.5.16
用户用的打印函数
 *======================================================================*/
PUBLIC void sys_udisp_int(int arg)
{
	disp_int(arg);
	return ;
}

/*======================================================================*
                           sys_udisp_str		add by visual 2016.5.16
用户用的打印函数
 *======================================================================*/
PUBLIC void sys_udisp_str(char *arg)
{
	disp_str(arg);
	return ;
}
/*======================================================================*
*                          sys_exec		被放在exec.c文件中
*======================================================================*/