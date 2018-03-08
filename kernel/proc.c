
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
