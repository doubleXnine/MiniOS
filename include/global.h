
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "fs.h"

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

/* equal to 1 if kernel is initializing, equal to 0 if done.
 * added by xw, 18/5/31
 */
EXTERN	int		kernel_initial;

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
EXTERN	PROCESS*	p_proc_next;	//the next process that will run. added by xw, 18/4/26

extern	PROCESS		cpu_table[];	//added by xw, 18/6/1
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

/// added by zcr
EXTERN u8 *		fsbuf;
EXTERN	const int	FSBUF_SIZE;
EXTERN	MESSAGE		fs_msg;

EXTERN	PROCESS * pcaller;
EXTERN	struct inode *	root_inode;

/* FS */
EXTERN	struct file_desc	f_desc_table[NR_FILE_DESC];
EXTERN	struct inode		inode_table[NR_INODE];
EXTERN	struct super_block	super_block[NR_SUPER_BLOCK];
//~zcr
