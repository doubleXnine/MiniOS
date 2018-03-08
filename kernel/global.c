
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"


PUBLIC	PROCESS			proc_table[NR_PCBS];										//edit by visual 2016.4.5	

//PUBLIC	char			task_stack[STACK_SIZE_TOTAL]; //delete  by viusal 2016.4.28

PUBLIC	TASK	task_table[NR_TASKS] = {{TestA, STACK_SIZE_TASK, "TestA"},			//edit by visual 2016.4.5	
										{TestB, STACK_SIZE_TASK, "TestB"},	
										{TestC, STACK_SIZE_TASK, "TestC"}};	


PUBLIC	irq_handler		irq_table[NR_IRQ];

PUBLIC	system_call		sys_call_table[NR_SYS_CALL] = {	sys_get_ticks, 
														sys_get_pid,		//add by visual 2016.4.6
														sys_kmalloc,		//add by visual 2016.4.6 
														sys_kmalloc_4k,		//add by visual 2016.4.7
														sys_malloc,			//add by visual 2016.4.7 
														sys_malloc_4k,		//add by visual 2016.4.7 
														sys_free,			//add by visual 2016.4.7 
														sys_free_4k,		//add by visual 2016.4.7 
														sys_fork,			//add by visual 2016.4.8 
														sys_pthread,			//add by visual 2016.4.11 
														sys_udisp_int,			//add by visual 2016.5.16 
														sys_udisp_str,			//add by visual 2016.5.16
														sys_exec			//add by visual 2016.5.16
														};			
