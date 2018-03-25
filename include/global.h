
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;
EXTERN	u8		gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	u8		idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	u32		k_reenter;
EXTERN  int     u_proc_sum; 		//内核中用户进程/线程数量 add by visual 2016.5.25

EXTERN	TSS		tss;
EXTERN	PROCESS*	p_proc_current;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern  TASK        task_table[];
extern	irq_handler	irq_table[];


//EXTERN	u32 PageTblNum;		//页表数量		add by visual 2016.4.5
EXTERN	u32 cr3_ready;		//当前进程的页目录		add by visual 2016.4.5

struct memfree{
	u32	addr;
	u32	size;
};