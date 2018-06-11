
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

//added by zcr
PUBLIC void	disable_irq(int irq);
PUBLIC void	enable_irq(int irq);
PUBLIC void	disable_int();
PUBLIC void	enable_int();
PUBLIC void	port_read(u16 port, void* buf, int n);
PUBLIC void	port_write(u16 port, void* buf, int n);
PUBLIC void	glitter(int row, int col);
//~zcr

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void sys_call();            /* syscall int_handler */
u32  read_cr2();			//add by visual 2016.5.9
u32  read_cr3();			//added by xw, 18/6/2
void refresh_page_cache();  //add by visual 2016.5.12
// void restart_int();
// void save_context();
void restart_initial();		//added by xw, 18/4/18
void restart_restore();		//added by xw, 18/4/20
void sched();				//added by xw, 18/4/18
void cleari();				//added by xw, 18/5/31
void seti();				//added by xw, 18/5/31
void halt();                //added by xw, 18/6/11

/* ktest.c */
void TestA();
void TestB();
void TestC();
void initial();
 
/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);

/***************************************************************
* 以下是系统调用相关函数的声明	
****************************************************************/
/* system call interface to processes */
/* syscall.asm */
PUBLIC int   get_ticks();
PUBLIC int   get_pid();					//add by visual 2016.4.6
PUBLIC void* kmalloc(int size);			//edit by visual 2016.5.9
PUBLIC void* kmalloc_4k();				//edit by visual 2016.5.9
PUBLIC void* malloc(int size);			//edit by visual 2016.5.9
PUBLIC void* malloc_4k();				//edit by visual 2016.5.9
PUBLIC int free(void *arg);				//edit by visual 2016.5.9
PUBLIC int free_4k(void* AdddrLin);		//edit by visual 2016.5.9
PUBLIC int fork();						//add by visual 2016.4.8
PUBLIC int pthread(void *arg);			//add by visual 2016.4.11
PUBLIC void* udisp_int(int arg);		//add by visual 2016.5.16
PUBLIC void* udisp_str(char* arg);	//add by visual 2016.5.16
PUBLIC u32 exec(char* path);		//add by visual 2016.5.16
PUBLIC void yield();				//added by xw, 18/4/19
PUBLIC void sleep(int n);			//added by xw, 18/4/19
PUBLIC void print_E();
PUBLIC void print_F();

/* system call implementation in kernel */
/* syscallc.c */		//edit by visual 2016.4.6
PUBLIC int   sys_get_ticks();           /* sys_call */
PUBLIC int   sys_get_pid();				//add by visual 2016.4.6
PUBLIC void* sys_kmalloc(int size);			//edit by visual 2016.5.9
PUBLIC void* sys_kmalloc_4k();				//edit by visual 2016.5.9
PUBLIC void* sys_malloc(int size);			//edit by visual 2016.5.9
PUBLIC void* sys_malloc_4k();				//edit by visual 2016.5.9
PUBLIC int sys_free(void *arg);				//edit by visual 2016.5.9
PUBLIC int sys_free_4k(void* AdddrLin);		//edit by visual 2016.5.9
PUBLIC int sys_pthread(void *arg);		//add by visual 2016.4.11
PUBLIC void sys_udisp_int(int arg);		//add by visual 2016.5.16
PUBLIC void sys_udisp_str(char* arg);		//add by visual 2016.5.16

/* proc.c */
PUBLIC void sys_yield();
PUBLIC void sys_sleep(int n);
PUBLIC void sys_wakeup(void *channel);
PUBLIC void sys_print_E();
PUBLIC void sys_print_F();
PUBLIC void preempt_enable();	//added by xw, 18/5/25
PUBLIC void preempt_disable();	//added by xw, 18/5/25
PUBLIC void inform_int();		//added by xw, 18/6/2

/*exec.c*/
PUBLIC u32 sys_exec(char* path);		//add by visual 2016.5.23
/*fork.c*/
PUBLIC int sys_fork();					//add by visual 2016.5.25

/***************************************************************
* 以上是系统调用相关函数的声明	
****************************************************************/

/*pagepte.c*/
PUBLIC	u32 init_page_pte(u32 pid);	//edit by visual 2016.4.28
PUBLIC 	void page_fault_handler(u32 vec_no, u32 err_code, u32 eip, u32 cs, u32 eflags);//add by visual 2016.4.19
PUBLIC	u32 get_pde_index(u32 AddrLin);//add by visual 2016.4.28
PUBLIC 	u32 get_pte_index(u32 AddrLin);
PUBLIC 	u32 get_pde_phy_addr(u32 pid);
PUBLIC 	u32 get_pte_phy_addr(u32 pid,u32 AddrLin);
PUBLIC  u32 get_page_phy_addr(u32 pid,u32 AddrLin);//线性地址
PUBLIC 	u32 pte_exist(u32 PageTblAddrPhy,u32 AddrLin);
PUBLIC 	u32 phy_exist(u32 PageTblPhyAddr,u32 AddrLin);
PUBLIC 	void write_page_pde(u32 PageDirPhyAddr,u32	AddrLin,u32 TblPhyAddr,u32 Attribute);
PUBLIC  void write_page_pte(	u32 TblPhyAddr,u32	AddrLin,u32 PhyAddr,u32 Attribute);
PUBLIC  u32 vmalloc(u32 size);
PUBLIC  int lin_mapping_phy(u32 AddrLin,u32 phy_addr,u32 pid,u32 pde_Attribute,u32 pte_Attribute);//edit by visual 2016.5.19
PUBLIC	void clear_kernel_pagepte_low();		//add by visual 2016.5.12

//added by xw, 18/6/2
// hd.c
PUBLIC void hd_handler(int irq);
//~xw

//zcr added(from ch9/e/include/sys/proto.h)
// fs.c
PUBLIC int		do_stat();
PUBLIC int		search_file(char * path);

PUBLIC struct inode * get_inode(int dev, int num);
PUBLIC void			put_inode(struct inode * pinode);
PUBLIC void			sync_inode(struct inode * p);
PUBLIC struct super_block *	get_super_block(int dev);

PUBLIC int		do_open();
PUBLIC int		do_close(int fd);
//~zcr
