
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clock.c
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
                           clock_handler
 *======================================================================*/
PUBLIC void clock_handler(int irq)
{
	ticks++;
	p_proc_current->task.ticks--;
	sys_wakeup(&ticks);

	//to make syscall reenter, deleted by xw, 17/12/11
	/*
	if (k_reenter != 0) {
		return;
	}
	*/

	if (p_proc_current->task.ticks > 0) {
		//do you know why this statement is added here? p_proc_current doesn't change if schedule() below 
		//isn't called. if you know the reason, please contact me at dongxuwei@163.com. added by xw, 17/12/16
		cr3_ready = p_proc_current->task.cr3;  //add by visual 2016.5.26		
		return;
	}

	schedule();
	cr3_ready = p_proc_current->task.cr3;			//add by visual 2016.4.5
}

/*======================================================================*
                              milli_delay
 *======================================================================*/
PUBLIC void milli_delay(int milli_sec)
{
        int t = get_ticks();

        while(((get_ticks() - t) * 1000 / HZ) < milli_sec) {}
}
