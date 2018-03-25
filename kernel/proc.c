
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

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;

	while (!greatest_ticks) 
	{
		for (p = proc_table; p < proc_table+NR_PCBS; p++)		//edit by visual 2016.4.5
		{
			if (p->task.stat == READY && p->task.ticks > greatest_ticks)  //edit by visual 2016.4.5
			{
				greatest_ticks = p->task.ticks;
				p_proc_current = p;
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
	p_proc_current->task.ticks--;
	save_context();
}

//used for processes to sleep for n ticks
PUBLIC void sys_sleep(int n)
{
	int ticks0;
	
	ticks0 = ticks;
	
	while(ticks - ticks0 < n){
		p_proc_current->task.channel = &ticks;
		p_proc_current->task.stat = SLEEPING;
		save_context();
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