
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "fs.h"		//added by zcr

//modified by xw, 18/6/2
//EXTERN HD_INT_WAITING_FLAG;		//added by zcr
EXTERN int volatile HD_INT_WAITING_FLAG;
//~xw

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;
/*	
	//shouldn't choose a new process when kernel is initializing. added by xw, 18/6/1
	if(kernel_initial == 1){
		p_proc_next = p_proc_current;
		return;
	}
*/	
	//Added by xw, 18/5/25
	if (p_proc_current->task.kernel_preemption == 0) {	//if kernel preemtion is off		
		p_proc_next = p_proc_current;
		return;
	}
	
	//Added by xw, 18/4/21
	if (p_proc_current->task.stat == READY && p_proc_current->task.ticks > 0) {		
		p_proc_next = p_proc_current;	//added by xw, 18/4/26
		return;
	}

	while (!greatest_ticks) 
	{
		for (p = proc_table; p < proc_table+NR_PCBS; p++)		//edit by visual 2016.4.5
		{
			if (p->task.stat == READY && p->task.ticks > greatest_ticks)  //edit by visual 2016.4.5
			{
				greatest_ticks = p->task.ticks;
//				p_proc_current = p;
				p_proc_next	= p;	//modified by xw, 18/4/26
			}

		}

		if (!greatest_ticks) 
		{
			for (p = proc_table; p < proc_table+NR_PCBS; p++) //edit by visual 2016.4.5
			{
				p->task.ticks = p->task.priority;
			}
		}
	}
	
	return;
}


/*======================================================================*
                           sys_get_ticks  delete by visual 2016.4.6
 *======================================================================*/
//PUBLIC int sys_get_ticks()
//{
//	return ticks;
//}

/*======================================================================*
                           alloc_PCB  add by visual 2016.4.8
 *======================================================================*/
PUBLIC PROCESS* alloc_PCB()
{//分配PCB表
	 PROCESS* p;
	 int i;
	 p=proc_table+NR_K_PCBS;//跳过前NR_K_PCBS个
	 for(i=NR_K_PCBS;i<NR_PCBS;i++)
	 {
	   if(p->task.stat==IDLE)break;
	   p++;	
	 }
	if(i>=NR_PCBS)	return 0;   //NULL
	else	return p;
}

/*======================================================================*
                           free_PCB  add by visual 2016.4.8
 *======================================================================*/
PUBLIC void free_PCB(PROCESS *p)
{//释放PCB表
	p->task.stat=IDLE;
}

/*======================================================================*
                           yield and sleep
 *======================================================================*/
//used for processes to give up the CPU
PUBLIC void sys_yield()
{
//	p_proc_current->task.ticks--;
	p_proc_current->task.ticks = 0;	/* modified by xw, 18/4/27 */
//	save_context();
	sched();	//Modified by xw, 18/4/19
}

//used for processes to sleep for n ticks
PUBLIC void sys_sleep(int n)
{
	int ticks0;
	
	ticks0 = ticks;
	p_proc_current->task.channel = &ticks;
	
	while(ticks - ticks0 < n){
		p_proc_current->task.stat = SLEEPING;
//		save_context();
		sched();	//Modified by xw, 18/4/19
	}
}

/*invoked by clock-interrupt handler to wakeup 
 *processes sleeping on ticks.
 */
PUBLIC void sys_wakeup(void *channel)
{
	PROCESS *p;
	
	for(p = proc_table; p < proc_table + NR_PCBS; p++){
		if(p->task.stat == SLEEPING && p->task.channel == channel){
			p->task.stat = READY;
		}
	}
}

/* 
 * This syscall needs long time to finish, so we can use it 
 * to check if our os is kernel-preemptive.
 * added by xw, 18/4/27
 */
PUBLIC void sys_print_E()
{		
	int i, j;
	
	disp_str("E( ");
	
	i = 100;
	while(--i){
		j = 1000;
		while(--j){}
	}
	
	disp_str(") ");
}

/* 
 * This syscall needs long time to finish, so we can use it
 * to check if our os is kernel-preemptive.
 * added by xw, 18/4/27
 */
PUBLIC void sys_print_F()
{
	int i, j;
	
	disp_str("F( ");
	
	i = 100;
	while(--i){
		j = 1000;
		while(--j){}
	}
	
	disp_str(") ");
}

/* 
 * declaration is in proto.h. added by xw, 18/5/25
 */
PUBLIC void preempt_enable()
{
	p_proc_current->task.kernel_preemption = 1;	
}

PUBLIC void preempt_disable()
{
	p_proc_current->task.kernel_preemption = 0;	
}

/// zcr copied from ch9
/*****************************************************************************
 *                                inform_int
 *****************************************************************************/
/**
 * <Ring 0> Inform a proc that an interrupt has occured.
 * 
 * @param task_nr  The task which will be informed.
 *****************************************************************************/
PUBLIC void inform_int()
{
	HD_INT_WAITING_FLAG = 0;
}

//modified by xw, 18/6/3
//PUBLIC int ldt_seg_linear(struct s_proc* p, int idx)
PUBLIC int ldt_seg_linear(PROCESS *p, int idx)
{
//	struct s_descriptor * d = &p->ldts[idx];
	struct s_descriptor * d = &p->task.ldts[idx];
	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}
//~xw

PUBLIC void* va2la(int pid, void* va)
{
	if(kernel_initial == 1){
		return va;
	}
	
	//struct s_proc* p = &proc_table[pid];
	PROCESS* p = &proc_table[pid];	//modified by xw, 18/6/3
	u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;
	
	return (void*)la;
}
//~zcr

