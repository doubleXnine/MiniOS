
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            const.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/// zcr copy some parts from chapter9/e/include/const.h

#ifndef	_ORANGES_CONST_H_
#define	_ORANGES_CONST_H_

/*最大整数定义*/
#define MAX_UNSIGNED_INT 0xFFFFFFFF		//最大的无符号整形
#define MAX_INT 0x7FFFFFFF			//最大的整形数

/* EXTERN */
#define	EXTERN	extern	/* EXTERN is defined as extern except in global.c */

/* 函数类型 */
#define	PUBLIC		/* PUBLIC is the opposite of PRIVATE */
#define	PRIVATE	static	/* PRIVATE x limits the scope of x */

//added by zcr
#define	STR_DEFAULT_LEN	1024
/* max() & min() */
#define	max(a,b)	((a) > (b) ? (a) : (b))
#define	min(a,b)	((a) < (b) ? (a) : (b))
//~zcr

/* Boolean */
#define	TRUE	1
#define	FALSE	0

//added by zcr
/* Color */
/*
 * e.g. MAKE_COLOR(BLUE, RED)
 *      MAKE_COLOR(BLACK, RED) | BRIGHT
 *      MAKE_COLOR(BLACK, RED) | BRIGHT | FLASH
 */
#define BLACK   0x0     /* 0000 */
#define WHITE   0x7     /* 0111 */
#define RED     0x4     /* 0100 */
#define GREEN   0x2     /* 0010 */
#define BLUE    0x1     /* 0001 */
#define FLASH   0x80    /* 1000 0000 */
#define BRIGHT  0x08    /* 0000 1000 */
#define	MAKE_COLOR(x,y)	((x<<4) | y) /* MAKE_COLOR(Background,Foreground) */
//~zcr

/* GDT 和 IDT 中描述符的个数 */
#define	GDT_SIZE	128
#define	IDT_SIZE	256

/* 权限 */
#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3
/* RPL */
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3

/// zcr added
/* TTY */
#define NR_CONSOLES	3	/* consoles */
//~zcr

/* 8259A interrupt controller ports. */
#define	INT_M_CTL	0x20	/* I/O port for interrupt controller         <Master> */
#define	INT_M_CTLMASK	0x21	/* setting bits in this port disables ints   <Master> */
#define	INT_S_CTL	0xA0	/* I/O port for second interrupt controller  <Slave>  */
#define	INT_S_CTLMASK	0xA1	/* setting bits in this port disables ints   <Slave>  */

/* 8253/8254 PIT (Programmable Interval Timer) */
#define TIMER0         0x40 /* I/O port for timer channel 0 */
#define TIMER_MODE     0x43 /* I/O port for timer mode control */
#define RATE_GENERATOR 0x34 /* 00-11-010-0 :
			     * Counter0 - LSB then MSB - rate generator - binary
			     */
#define TIMER_FREQ     1193182L/* clock frequency for timer in PC and AT */
#define HZ             100  /* clock freq (software settable on IBM-PC) */

//added by zcr
/* AT keyboard */
/* 8042 ports */
#define KB_DATA		0x60	/* I/O port for keyboard data
					Read : Read Output Buffer
					Write: Write Input Buffer(8042 Data&8048 Command) */
#define KB_CMD		0x64	/* I/O port for keyboard command
					Read : Read Status Register
					Write: Write Input Buffer(8042 Command) */
#define LED_CODE	0xED
#define KB_ACK		0xFA

/* VGA */
#define	CRTC_ADDR_REG	0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	0x3D5	/* CRT Controller Registers - Data Register */
#define	START_ADDR_H	0xC	/* reg index of video mem start addr (MSB) */
#define	START_ADDR_L	0xD	/* reg index of video mem start addr (LSB) */
#define	CURSOR_H	0xE	/* reg index of cursor position (MSB) */
#define	CURSOR_L	0xF	/* reg index of cursor position (LSB) */
#define	V_MEM_BASE	0xB8000	/* base of color video memory */
#define	V_MEM_SIZE	0x8000	/* 32K: B8000H -> BFFFFH */
//~zcr

/* Hardware interrupts */
#define	NR_IRQ		16	/* Number of IRQs */
#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ	5	/* xt winchester */
#define	FLOPPY_IRQ	6	/* floppy disk */
#define	PRINTER_IRQ	7
#define	AT_WINI_IRQ	14	/* at winchester */

//added by zcr
/* tasks */
/* 注意 TASK_XXX 的定义要与 global.c 中对应 */
#define INVALID_DRIVER	-20
#define INTERRUPT	-10
#define TASK_A	    0 
// #define TASK_MM	4
// #define ANY		(NR_TASKS + NR_PROCS + 10)
// #define NO_TASK	(NR_TASKS + NR_PROCS + 20)
//~zcr

/* system call */
#define NR_SYS_CALL     17

//added by zcr
/* ipc */
#define SEND		1
#define RECEIVE		2
#define BOTH		3	/* BOTH = (SEND | RECEIVE) */

/* magic chars used by `printx' */
#define MAG_CH_PANIC	'\002'
#define MAG_CH_ASSERT	'\003'

/**
 * @enum msgtype
 * @brief MESSAGE types
 */
enum msgtype {
	/* 
	 * when hard interrupt occurs, a msg (with type==HARD_INT) will
	 * be sent to some tasks
	 */
	HARD_INT = 1,

	/* SYS task */
	GET_TICKS,

	/// zcr added from ch9/e/include/const.h
	/* FS */
	OPEN, CLOSE, READ, WRITE, LSEEK, STAT, UNLINK,
	
	/* message type for drivers */
	DEV_OPEN = 1001,
	DEV_CLOSE,
	DEV_READ,
	DEV_WRITE,
	DEV_IOCTL
};

/* macros for messages */
#define	FD		u.m3.m3i1 
#define	PATHNAME	u.m3.m3p1 
#define	FLAGS		u.m3.m3i1 
#define	NAME_LEN	u.m3.m3i2 
#define	CNT		u.m3.m3i2
#define	REQUEST		u.m3.m3i2
#define	PROC_NR		u.m3.m3i3
#define	DEVICE		u.m3.m3i4
#define	POSITION	u.m3.m3l1
#define	BUF		u.m3.m3p2
#define	OFFSET		u.m3.m3i2 
#define	WHENCE		u.m3.m3i3 

/* #define	PID		u.m3.m3i2 */
/* #define	STATUS		u.m3.m3i1 */
#define	RETVAL		u.m3.m3i1
/* #define	STATUS		u.m3.m3i1 */


#define	DIOCTL_GET_GEO	1

/* Hard Drive */
#define SECTOR_SIZE		512
#define SECTOR_BITS		(SECTOR_SIZE * 8)
#define SECTOR_SIZE_SHIFT	9

/* major device numbers (corresponding to kernel/global.c::dd_map[]) */
#define	NO_DEV			0
#define	DEV_FLOPPY		1
#define	DEV_CDROM		2
#define	DEV_HD			3
#define	DEV_CHAR_TTY		4
#define	DEV_SCSI		5
/* make device number from major and minor numbers */
#define	MAJOR_SHIFT		8
#define	MAKE_DEV(a,b)		((a << MAJOR_SHIFT) | b)
/* separate major and minor numbers from device number */
#define	MAJOR(x)		((x >> MAJOR_SHIFT) & 0xFF)
#define	MINOR(x)		(x & 0xFF)

#define	INVALID_INODE		0
#define	ROOT_INODE		1

#define	MAX_DRIVES		2
#define	NR_PART_PER_DRIVE	4
#define	NR_SUB_PER_PART		16
#define	NR_SUB_PER_DRIVE	(NR_SUB_PER_PART * NR_PART_PER_DRIVE)
#define	NR_PRIM_PER_DRIVE	(NR_PART_PER_DRIVE + 1)

/**
 * @def MAX_PRIM
 * Defines the max minor number of the primary partitions.
 * If there are 2 disks, prim_dev ranges in hd[0-9], this macro will
 * equals 9.
 */
#define	MAX_PRIM		(MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)

#define	MAX_SUBPARTITIONS	(NR_SUB_PER_DRIVE * MAX_DRIVES)

/* device numbers of hard disk */
#define	MINOR_hd1a		0x10
#define	MINOR_hd2a		(MINOR_hd1a+NR_SUB_PER_PART)

#define	MINOR_BOOT		MINOR_hd2a	/// added by zcr
#define	ROOT_DEV		MAKE_DEV(DEV_HD, MINOR_BOOT)

#define	P_PRIMARY	0
#define	P_EXTENDED	1

#define ORANGES_PART	0x99	/* Orange'S partition */
#define NO_PART		0x00	/* unused entry */
#define EXT_PART	0x05	/* extended partition */

#define	NR_FILES	64
#define	NR_FILE_DESC	64	/* FIXME */
#define	NR_INODE	64	/* FIXME */
#define	NR_SUPER_BLOCK	8


/* INODE::i_mode (octal, lower 32 bits reserved) */
#define I_TYPE_MASK     0170000
#define I_REGULAR       0100000
#define I_BLOCK_SPECIAL 0060000
#define I_DIRECTORY     0040000
#define I_CHAR_SPECIAL  0020000
#define I_NAMED_PIPE	0010000

#define	is_special(m)	((((m) & I_TYPE_MASK) == I_BLOCK_SPECIAL) ||	\
			 (((m) & I_TYPE_MASK) == I_CHAR_SPECIAL))

#define	NR_DEFAULT_FILE_SECTS	2048 /* 2048 * 512 = 1MB */
//~zcr

/*页表相关*/
#define	PageTblNumAddr		0x500	//页表数量放在这个位置,必须与load.inc中一致					add by visual 2016.5.11
#define KernelPageTblAddr	0x200000 //内核页表物理地址，必须与load.inc中一致			add by visual 2016.5.17
/*线性地址描述*/	//edit by visual 2016.5.25
#define	KernelSize			0x800000 			//内核的大小//add by visual 2016.5.10
#define K_PHY2LIN(x)		((x)+0xC0000000)	//内核中物理地址转线性地址		//add by visual 2016.5.10
#define num_4B	0x4			//4B大小
#define num_1K	0x400		//1k大小
#define num_4K	0x1000		//4k大小
#define num_4M	0x400000	//4M大小
#define TextLinBase 			((u32)0x0) 						//进程代码的起始地址，这是参考值，具体以elf描述为准
#define TextLinLimitMAX   		(TextLinBase+0x20000000)  	//大小：512M，这是参考值，具体以elf描述为准，
#define DataLinBase 			TextLinLimitMAX 			//进程数据的起始地址，这是参考值，具体以elf描述为准
#define DataLinLimitMAX   		(DataLinBase+0x20000000)  	//大小：512M，这是参考值，具体以elf描述为准，但是代码和数据长度总和不能超过这个值
#define VpageLinBase 			DataLinLimitMAX 			//保留内存起始地址
#define VpageLinLimitMAX  		(VpageLinBase+0x8000000-num_4K) //大小：128M-4k
#define SharePageBase			VpageLinLimitMAX			//共享页线性地址，执行fork\pthread的时候用,共享页必须4K对齐
#define SharePageLimit			(SharePageBase+num_4K)		//大小：4k
#define HeapLinBase 			SharePageLimit	 			//堆的起始地址
#define HeapLinLimitMAX  		(HeapLinBase+0x40000000)  	//大小：1G
#define StackLinLimitMAX		HeapLinLimitMAX				//栈的大小： 1G-128M-4K（注意栈的基址和界限方向）
#define StackLinBase			(ArgLinBase-num_4B)			//=(StackLinLimitMAX+1G-128M-4K-4B)栈的起始地址,放在参数位置之前（注意堆栈的增长方向）
#define ArgLinBase 				(KernelLinBase-0x1000)		//参数存放位置起始地址，放在3G前，暂时还没没用到
#define ArgLinLimitMAX  		KernelLinBase  				//=(ArgLinBase+0x1000)大小：4K。
#define	KernelLinBase			0xC0000000 					//内核线性起始地址(有0x30400的偏移)
#define	KernelLinLimitMAX		(KernelLinBase+0x40000000) 	//大小：1G

/***************目前线性地址布局*****************************		edit by visual 2016.5.25
*				进程代码		0 ~ 512M ,限制大小为512M
*				进程数据		512M ~ 1G，限制大小为512M
*				进程保留内存（以后可能存放虚页表和其他一些信息） 1G ~ 1G+128M，限制大小为128M,共享页放在这个位置	
*				进程堆			1G+128M ~ 2G+128M，限制大小为1G				
*				进程栈			2G+128M ~ 3G-4K,限制大小为 1G-128M-4K
*				进程参数		3G-4K~3G，限制大小为4K
*				内核			3G~4G，限制大小为1G
***********************************************************/

//#define ShareTblLinAddr			(KernelLinLimitMAX-0x1000)	//公共临时共享页，放在内核最后一个页表的最后一项上
	
/*分页机制常量的定义,必须与load.inc中一致*/				//add by visual 2016.4.5		
#define	PG_P		1	// 页存在属性位
#define	PG_RWR		0	// R/W 属性位值, 读/执行
#define	PG_RWW		2	// R/W 属性位值, 读/写/执行
#define	PG_USS		0	// U/S 属性位值, 系统级
#define	PG_USU		4	// U/S 属性位值, 用户级
#define PG_PS		64	// PS属性位值，4K页



#endif /* _ORANGES_CONST_H_ */
