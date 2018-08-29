/*****************************************************
*			fork.c			//add by visual 2016.5.25
*系统调用fork()功能实现部分sys_fork()
********************************************************/
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PRIVATE int fork_mem_cpy(u32 ppid,u32 pid);
PRIVATE int fork_pcb_cpy(PROCESS* p_child);
PRIVATE int fork_update_info(PROCESS* p_child);


/**********************************************************
*		sys_fork			//add by visual 2016.5.25
*系统调用sys_fork的具体实现部分
*************************************************************/
PUBLIC int sys_fork()
{
	PROCESS* p_child;
	char* p_reg;	//point to a register in the new kernel stack, added by xw, 17/12/11
	
	/*****************申请空白PCB表**********************/
	p_child = alloc_PCB();
	if( 0==p_child )
	{
		disp_color_str("PCB NULL,fork faild!",0x74);
		return -1;
	}
	else
	{
		/****************初始化子进程高端地址页表（内核部分）***********************///这个页表可以复制父进程的！
		init_page_pte(p_child->task.pid);	//这里面已经填写了该进程的cr3寄存器变量		
		
		/************复制父进程的PCB部分内容（保留了自己的标识信息）**************/
		fork_pcb_cpy(p_child);

		/**************复制线性内存，包括堆、栈、代码数据等等***********************/
		fork_mem_cpy(p_proc_current->task.pid,p_child->task.pid);
		
		/**************更新进程树标识info信息************************/
		fork_update_info(p_child);
	
		/************修改子进程的名字***************/		
		strcpy(p_child->task.p_name,"fork");	// 所有的子进程都叫fork
		
		/*************子进程返回值在其eax寄存器***************/
		p_child->task.regs.eax = 0;//return child with 0
		p_reg = (char*)(p_child + 1);	//added by xw, 17/12/11
		*((u32*)(p_reg + EAXREG - P_STACKTOP)) = p_child->task.regs.eax;	//added by xw, 17/12/11

		/****************用户进程数+1****************************/
		u_proc_sum += 1;

		disp_color_str("[fork success:",0x72);
		disp_color_str(p_proc_current->task.p_name,0x72);
		disp_color_str("]",0x72);
		
		//anything child need is prepared now, set its state to ready. added by xw, 17/12/11
		p_child->task.stat = READY;
	}
	return p_child->task.pid;	
}


/**********************************************************
*		fork_mem_cpy			//add by visual 2016.5.24
*复制父进程的一系列内存数据
*************************************************************/
PRIVATE int fork_mem_cpy(u32 ppid,u32 pid)
{
	u32 addr_lin;
	//复制代码，代码是共享的，直接将物理地址挂载在子进程的页表上
	for(addr_lin = p_proc_current->task.memmap.text_lin_base ; addr_lin < p_proc_current->task.memmap.text_lin_limit ; addr_lin+=num_4K )
	{
		lin_mapping_phy(addr_lin,//线性地址
						get_page_phy_addr(ppid,addr_lin),//物理地址，为MAX_UNSIGNED_INT时，由该函数自动分配物理内存
						pid,//要挂载的进程的pid，子进程的pid
						PG_P  | PG_USU | PG_RWW,//页目录属性，一般都为可读写
						PG_P  | PG_USU | PG_RWR);//页表属性，代码是只读的
	}
	//复制数据，数据不共享，子进程需要申请物理地址，并复制过来
	for(addr_lin = p_proc_current->task.memmap.data_lin_base ; addr_lin < p_proc_current->task.memmap.data_lin_limit ; addr_lin+=num_4K )
	{	
		lin_mapping_phy(SharePageBase,0,ppid,PG_P  | PG_USU | PG_RWW,0);//使用前必须清除这个物理页映射
		//disp_str("#");
		lin_mapping_phy(SharePageBase,MAX_UNSIGNED_INT,ppid,PG_P  | PG_USU | PG_RWW,PG_P  | PG_USU | PG_RWW);//利用父进程的共享页申请物理页
		memcpy((void*)SharePageBase,(void*)(addr_lin&0xFFFFF000),num_4K);//将数据复制到物理页上,注意这个地方是强制一页一页复制的
		lin_mapping_phy(addr_lin,//线性地址
						get_page_phy_addr(ppid,SharePageBase),//物理地址，获取共享页的物理地址，填进子进程页表
						pid,//要挂载的进程的pid，子进程的pid
						PG_P  | PG_USU | PG_RWW,//页目录属性，一般都为可读写
						PG_P  | PG_USU | PG_RWW);//页表属性，数据是可读写的		
	}
	//复制保留内存，保留内存不共享，子进程需要申请物理地址，并复制过来
	for(addr_lin = p_proc_current->task.memmap.vpage_lin_base ; addr_lin < p_proc_current->task.memmap.vpage_lin_limit ; addr_lin+=num_4K )
	{
		lin_mapping_phy(SharePageBase,0,ppid,PG_P  | PG_USU | PG_RWW,0);//使用前必须清除这个物理页映射
		lin_mapping_phy(SharePageBase,MAX_UNSIGNED_INT,ppid,PG_P  | PG_USU | PG_RWW,PG_P  | PG_USU | PG_RWW);//利用父进程的共享页申请物理页
		memcpy((void*)SharePageBase,(void*)(addr_lin&0xFFFFF000),num_4K);//将数据复制到物理页上,注意这个地方是强制一页一页复制的
		lin_mapping_phy(addr_lin,//线性地址
						get_page_phy_addr(ppid,SharePageBase),//物理地址，获取共享页的物理地址，填进子进程页表
						pid,//要挂载的进程的pid，子进程的pid
						PG_P  | PG_USU | PG_RWW,//页目录属性，一般都为可读写
						PG_P  | PG_USU | PG_RWW);//页表属性，保留内存是可读写的		
	}
	
	//复制堆，堆不共享，子进程需要申请物理地址，并复制过来
	for(addr_lin = p_proc_current->task.memmap.heap_lin_base ; addr_lin < p_proc_current->task.memmap.heap_lin_limit ; addr_lin+=num_4K )
	{
		lin_mapping_phy(SharePageBase,0,ppid,PG_P  | PG_USU | PG_RWW,0);//使用前必须清除这个物理页映射
		lin_mapping_phy(SharePageBase,MAX_UNSIGNED_INT,ppid,PG_P  | PG_USU | PG_RWW,PG_P  | PG_USU | PG_RWW);//利用父进程的共享页申请物理页
		memcpy((void*)SharePageBase,(void*)(addr_lin&0xFFFFF000),num_4K);//将数据复制到物理页上,注意这个地方是强制一页一页复制的
		lin_mapping_phy(addr_lin,//线性地址
						get_page_phy_addr(ppid,SharePageBase),//物理地址，获取共享页的物理地址，填进子进程页表
						pid,//要挂载的进程的pid，子进程的pid
						PG_P  | PG_USU | PG_RWW,//页目录属性，一般都为可读写
						PG_P  | PG_USU | PG_RWW);//页表属性，堆是可读写的		
	}	
	
	//复制栈，栈不共享，子进程需要申请物理地址，并复制过来(注意栈的复制方向)
	for(addr_lin = p_proc_current->task.memmap.stack_lin_base ; addr_lin > p_proc_current->task.memmap.stack_lin_limit ; addr_lin-=num_4K )
	{
		lin_mapping_phy(SharePageBase,0,ppid,PG_P  | PG_USU | PG_RWW,0);//使用前必须清除这个物理页映射
		lin_mapping_phy(SharePageBase,MAX_UNSIGNED_INT,ppid,PG_P  | PG_USU | PG_RWW,PG_P  | PG_USU | PG_RWW);//利用父进程的共享页申请物理页
		memcpy((void*)SharePageBase,(void*)(addr_lin&0xFFFFF000),num_4K);//将数据复制到物理页上,注意这个地方是强制一页一页复制的
		lin_mapping_phy(addr_lin,//线性地址
						get_page_phy_addr(ppid,SharePageBase),//物理地址，获取共享页的物理地址，填进子进程页表
						pid,//要挂载的进程的pid，子进程的pid
						PG_P  | PG_USU | PG_RWW,//页目录属性，一般都为可读写
						PG_P  | PG_USU | PG_RWW);//页表属性，栈是可读写的		
	}
	
	//复制参数区，参数区不共享，子进程需要申请物理地址，并复制过来
	for(addr_lin = p_proc_current->task.memmap.arg_lin_base ; addr_lin < p_proc_current->task.memmap.arg_lin_limit ; addr_lin+=num_4K )
	{
		lin_mapping_phy(SharePageBase,0,ppid,PG_P  | PG_USU | PG_RWW,0);//使用前必须清除这个物理页映射
		lin_mapping_phy(SharePageBase,MAX_UNSIGNED_INT,ppid,PG_P  | PG_USU | PG_RWW,PG_P  | PG_USU | PG_RWW);//利用父进程的共享页申请物理页
		memcpy((void*)SharePageBase,(void*)(addr_lin&0xFFFFF000),num_4K);//将数据复制到物理页上,注意这个地方是强制一页一页复制的
		lin_mapping_phy(addr_lin,//线性地址
						get_page_phy_addr(ppid,SharePageBase),//物理地址，获取共享页的物理地址，填进子进程页表
						pid,//要挂载的进程的pid，子进程的pid
						PG_P  | PG_USU | PG_RWW,//页目录属性，一般都为可读写
						PG_P  | PG_USU | PG_RWW);//页表属性，参数区是可读写的		
	}
	return 0;		
}

/**********************************************************
*		fork_pcb_cpy			//add by visual 2016.5.26
*复制父进程PCB表，但是又马上恢复了子进程的标识信息
*************************************************************/
PRIVATE int fork_pcb_cpy(PROCESS* p_child)
{
	int pid;
	u32 eflags,selector_ldt,cr3_child;
	char* p_reg;	//point to a register in the new kernel stack, added by xw, 17/12/11
//	char* esp_save_int, esp_save_context;	//It's not what you want! damn it.
	char *esp_save_int, *esp_save_context;	//use to save corresponding field in child's PCB.
	
	//暂存标识信息
	pid = p_child->task.pid;
	
	//eflags = p_child->task.regs.eflags;
	p_reg = (char*)(p_child + 1);	//added by xw, 17/12/11
	eflags = *((u32*)(p_reg + EFLAGSREG - P_STACKTOP));	//added by xw, 17/12/11
	
	selector_ldt = p_child->task.ldt_sel;
	cr3_child = p_child->task.cr3; 
	
	//复制PCB内容 
	//modified by xw, 17/12/11
	//modified begin
	//*p_child = *p_proc_current;
	
	//esp_save_int and esp_save_context must be saved, because the child and the parent 
	//use different kernel stack! And these two are importent to the child's initial running.
	//Added by xw, 18/4/21
	esp_save_int = p_child->task.esp_save_int;
	esp_save_context = p_child->task.esp_save_context;
//	disp_str("<");
//	disp_int((int)(p_child->task.esp_save_context));
//	disp_str("> ");
	p_child->task = p_proc_current->task;
	//note that syscalls can be interrupted now! the state of child can only be setted
	//READY when anything else is well prepared. if an interruption happens right here,
	//an error will still occur.
	p_child->task.stat = IDLE;
	p_child->task.esp_save_int = esp_save_int;	//esp_save_int of child must be restored!!
	p_child->task.esp_save_context = esp_save_context;	//same above
//	p_child->task.esp_save_context = (char*)(p_child + 1) - P_STACKTOP - 4 * 6;	
	memcpy(((char*)(p_child + 1) - P_STACKTOP), ((char*)(p_proc_current + 1) - P_STACKTOP), 18 * 4);
	//modified end
	
	//恢复标识信息
	p_child->task.pid = pid;
	
	//p_child->task.regs.eflags = eflags;
	p_reg = (char*)(p_child + 1);	//added by xw, 17/12/11
	*((u32*)(p_reg + EFLAGSREG - P_STACKTOP)) = eflags;	//added by xw, 17/12/11
	
	p_child->task.ldt_sel = selector_ldt;				
	p_child->task.cr3 = cr3_child;
	return 0;
}



/**********************************************************
*		fork_update_info			//add by visual 2016.5.26
*更新父进程和子进程的进程树标识info
*************************************************************/
PRIVATE int fork_update_info(PROCESS* p_child)
{
	/************更新父进程的info***************/		
	//p_proc_current->task.info.type;		//当前是进程还是线程
	//p_proc_current->task.info.real_ppid;  //亲父进程，创建它的那个进程
	//p_proc_current->task.info.ppid;		//当前父进程	
	p_proc_current->task.info.child_p_num += 1; //子进程数量
	p_proc_current->task.info.child_process[p_proc_current->task.info.child_p_num-1] = p_child->task.pid;//子进程列表
	//p_proc_current->task.info.child_t_num;	//子线程数量
	//p_proc_current->task.info.child_thread[NR_CHILD_MAX];//子线程列表	
	//p_proc_current->task.text_hold;			//是否拥有代码
	//p_proc_current->task.data_hold;			//是否拥有数据
		
	/************更新子进程的info***************/	
	p_child->task.info.type = p_proc_current->task.info.type;	//当前进程属性跟父进程一样
	p_child->task.info.real_ppid = p_proc_current->task.pid;  //亲父进程，创建它的那个进程
	p_child->task.info.ppid = p_proc_current->task.pid;		//当前父进程	
	p_child->task.info.child_p_num = 0; //子进程数量
	//p_child->task.info.child_process[NR_CHILD_MAX] = pid;//子进程列表
	p_child->task.info.child_t_num = 0;	//子线程数量
	//p_child->task.info.child_thread[NR_CHILD_MAX];//子线程列表	
	p_child->task.info.text_hold = 0;			//是否拥有代码，子进程不拥有代码
	p_child->task.info.data_hold = 1;			//是否拥有数据，子进程拥有数据
	
	return 0;
}