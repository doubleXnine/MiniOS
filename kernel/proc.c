
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
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
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;
	
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
				// p_proc_current = p;
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
}

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

//added by zcr
PUBLIC int ldt_seg_linear(PROCESS *p, int idx)
{
	struct s_descriptor * d = &p->task.ldts[idx];
	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}

PUBLIC void* va2la(int pid, void* va)
{
	if(kernel_initial == 1){
		return va;
	}
	
	PROCESS* p = &proc_table[pid];
	u32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	u32 la = seg_base + (u32)va;
	
	return (void*)la;
}
//~zcr

