/// zcr copy from chapter9/d fs/main.c and modified it.

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "fs_const.h"
#include "hd.h"
#include "fs.h"
#include "fs_misc.h"

/* FSBUF_SIZE is defined as macro in fs_const.h.
 * The physical address space 6MB~7MB is used as fs buffer in Orange's, but we can't use this
 * space directly in minios. We allocate the fs buffer space in kernel initialization stage. 
 * modified by xw, 18/6/15
 */
// PUBLIC const int	FSBUF_SIZE	= 0x100000;
// PUBLIC u8 * fsbuf = (u8*)0x600000;

PUBLIC void mkfs();

PRIVATE void read_super_block(int dev);

/// added by zcr from chap9/e/fs/open.c
PRIVATE struct inode * create_file(char * path, int flags);
PRIVATE int alloc_imap_bit(int dev);
PRIVATE int alloc_smap_bit(int dev, int nr_sects_to_alloc);
PRIVATE struct inode * new_inode(int dev, int inode_nr, int start_sect);
PRIVATE void new_dir_entry(struct inode * dir_inode, int inode_nr, char * filename);

/// zcr added
PUBLIC void init_fs() 
{
	/// added by zcr
	disp_str("Initializing file system...  ");

	/// zcr copied from ch9/e/fs/main.c/init_fs()
	int i;
	for (i = 0; i < NR_FILE_DESC; i++)
		memset(&f_desc_table[i], 0, sizeof(struct file_desc));
	for (i = 0; i < NR_INODE; i++)
		memset(&inode_table[i], 0, sizeof(struct inode));
	struct super_block * sb = super_block;
	for (; sb < &super_block[NR_SUPER_BLOCK]; sb++)
		sb->sb_dev = NO_DEV;
	
	/* load super block of ROOT */
	read_super_block(ROOT_DEV);
	sb = get_super_block(ROOT_DEV);
	disp_str("Superblock Address:");
	disp_int(sb);
	disp_str(" \n");
	if(sb->magic != MAGIC_V1) {
		mkfs();
		disp_str("Make file system Done.\n");
		for (; sb < &super_block[NR_SUPER_BLOCK]; sb++)
			sb->sb_dev = NO_DEV;
		read_super_block(ROOT_DEV);
	}

	root_inode = get_inode(ROOT_DEV, ROOT_INODE);
}

/*****************************************************************************
 *                                mkfs
 *****************************************************************************/
/**
 * <Ring 1> Make a available Orange'S FS in the disk. It will
 *          - Write a super block to sector 1.
 *          - Create three special files: dev_tty0, dev_tty1, dev_tty2
 *          - Create the inode map
 *          - Create the sector map
 *          - Create the inodes of the files
 *          - Create `/', the root directory
 *****************************************************************************/
PUBLIC void mkfs()
{
	MESSAGE driver_msg;
	int i, j;

	int bits_per_sect = SECTOR_SIZE * 8; /* 8 bits per byte */

	/* get the geometry of ROOTDEV */
	struct part_info geo;
	driver_msg.type		= DEV_IOCTL;
	driver_msg.DEVICE	= MINOR(ROOT_DEV);
	driver_msg.REQUEST	= DIOCTL_GET_GEO;
	driver_msg.BUF		= &geo;
	driver_msg.PROC_NR	= proc2pid(p_proc_current);
	// assert(dd_map[MAJOR(ROOT_DEV)].driver_nr != INVALID_DRIVER);
	// send_recv(BOTH, dd_map[MAJOR(ROOT_DEV)].driver_nr, &driver_msg);
	hd_ioctl(&driver_msg);

	// printl("dev size: 0x%x sectors\n", geo.size);
	disp_str("dev size: ");
	disp_int(geo.size);
	disp_str(" sectors\n");

	/************************/
	/*      super block     */
	/************************/
	struct super_block sb;
	sb.magic	  = MAGIC_V1;
	sb.nr_inodes	  = bits_per_sect;
	sb.nr_inode_sects = sb.nr_inodes * INODE_SIZE / SECTOR_SIZE;
	sb.nr_sects	  = geo.size; /* partition size in sector */
	sb.nr_imap_sects  = 1;
	sb.nr_smap_sects  = sb.nr_sects / bits_per_sect + 1;
	sb.n_1st_sect	  = 1 + 1 +   /* boot sector & super block */
		sb.nr_imap_sects + sb.nr_smap_sects + sb.nr_inode_sects;
	sb.root_inode	  = ROOT_INODE;
	sb.inode_size	  = INODE_SIZE;
	struct inode x;
	sb.inode_isize_off= (int)&x.i_size - (int)&x;
	sb.inode_start_off= (int)&x.i_start_sect - (int)&x;
	sb.dir_ent_size	  = DIR_ENTRY_SIZE;
	struct dir_entry de;
	sb.dir_ent_inode_off = (int)&de.inode_nr - (int)&de;
	sb.dir_ent_fname_off = (int)&de.name - (int)&de;

	memset(fsbuf, 0x90, SECTOR_SIZE);
	memcpy(fsbuf, &sb, SUPER_BLOCK_SIZE);

	/* write the super block */
	WR_SECT(ROOT_DEV, 1);

	// printl("devbase:0x%x00, sb:0x%x00, imap:0x%x00, smap:0x%x00\n"
	//        "        inodes:0x%x00, 1st_sector:0x%x00\n", 
	//        geo.base * 2,
	//        (geo.base + 1) * 2,
	//        (geo.base + 1 + 1) * 2,
	//        (geo.base + 1 + 1 + sb.nr_imap_sects) * 2,
	//        (geo.base + 1 + 1 + sb.nr_imap_sects + sb.nr_smap_sects) * 2,
	//        (geo.base + sb.n_1st_sect) * 2);

	disp_str("devbase:");
	disp_int(geo.base * 2);
	disp_str("00");
	disp_str(" sb:");
	disp_int((geo.base + 1) * 2);
	disp_str("00");
	disp_str(" imap:");
	disp_int((geo.base + 1 + 1) * 2);
	disp_str("00");
	disp_str(" smap:");
	disp_int((geo.base + 1 + 1 + sb.nr_imap_sects) * 2);
	disp_str("00\n");
	disp_str("        inodes:");
	disp_int((geo.base + 1 + 1 + sb.nr_imap_sects + sb.nr_smap_sects) * 2);
	disp_str("00");
	disp_str(" 1st_sector:");
	disp_int((geo.base + sb.n_1st_sect) * 2);
	disp_str("00\n");

	/************************/
	/*       inode map      */
	/************************/
	memset(fsbuf, 0, SECTOR_SIZE);
	for (i = 0; i < (NR_CONSOLES + 2); i++)
		fsbuf[0] |= 1 << i;

	
	WR_SECT(ROOT_DEV, 2);

	/************************/
	/*      secter map      */
	/************************/
	memset(fsbuf, 0, SECTOR_SIZE);
	int nr_sects = NR_DEFAULT_FILE_SECTS + 1;
	/*             ~~~~~~~~~~~~~~~~~~~|~   |
	 *                                |    `--- bit 0 is reserved
	 *                                `-------- for `/'
	 */
	for (i = 0; i < nr_sects / 8; i++)
		fsbuf[i] = 0xFF;

	for (j = 0; j < nr_sects % 8; j++)
		fsbuf[i] |= (1 << j);

	WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sects);

	/* zeromemory the rest sector-map */
	memset(fsbuf, 0, SECTOR_SIZE);
	for (i = 1; i < sb.nr_smap_sects; i++)
		WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sects + i);

	/************************/
	/*       inodes         */
	/************************/
	/* inode of `/' */
	memset(fsbuf, 0, SECTOR_SIZE);
	struct inode * pi = (struct inode*)fsbuf;
	pi->i_mode = I_DIRECTORY;
	pi->i_size = DIR_ENTRY_SIZE * 4; /* 4 files: (预定义四个文件)
					  * `.',
					  * `dev_tty0', `dev_tty1', `dev_tty2',
					  */
	pi->i_start_sect = sb.n_1st_sect;
	pi->i_nr_sects = NR_DEFAULT_FILE_SECTS;
	/* inode of `/dev_tty0~2' */
	for (i = 0; i < NR_CONSOLES; i++) {
		pi = (struct inode*)(fsbuf + (INODE_SIZE * (i + 1)));
		pi->i_mode = I_CHAR_SPECIAL;
		pi->i_size = 0;
		pi->i_start_sect = MAKE_DEV(DEV_CHAR_TTY, i);
		pi->i_nr_sects = 0;
	}
	WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sects + sb.nr_smap_sects);

	/************************/
	/*          `/'         */
	/************************/
	memset(fsbuf, 0, SECTOR_SIZE);
	struct dir_entry * pde = (struct dir_entry *)fsbuf;

	pde->inode_nr = 1;
	strcpy(pde->name, ".");

	/* dir entries of `/dev_tty0~2' */
	for (i = 0; i < NR_CONSOLES; i++) {
		pde++;
		pde->inode_nr = i + 2; /* dev_tty0's inode_nr is 2 */
		// sprintf(pde->name, "dev_tty%d", i);
		/// zcr added to replace the statement above
		switch(i) {
			case 0:	
				strcpy(pde->name, "dev_tty0"); 
				break;
			case 1:
				strcpy(pde->name, "dev_tty1"); 
				break;
			case 2:
				strcpy(pde->name, "dev_tty2"); 
				break;
		}
	}
	WR_SECT(ROOT_DEV, sb.n_1st_sect);
}

/*****************************************************************************
 *                                rw_sector
 *****************************************************************************/
/**
 * <Ring 1> R/W a sector via messaging with the corresponding driver.
 * 
 * @param io_type  DEV_READ or DEV_WRITE
 * @param dev      device nr
 * @param pos      Byte offset from/to where to r/w.
 * @param bytes    r/w count in bytes.
 * @param proc_nr  To whom the buffer belongs.
 * @param buf      r/w buffer.
 * 
 * @return Zero if success.
 *****************************************************************************/
/// zcr: change the "u64 pos" to "int pos"
PUBLIC int rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr, void* buf)
{
	MESSAGE driver_msg;
	
	driver_msg.type		= io_type;
	driver_msg.DEVICE	= MINOR(dev);
	//attention
	// driver_msg.POSITION	= (unsigned long long)pos;
	driver_msg.POSITION	= pos;
	driver_msg.CNT		= bytes;	/// hu is: 512
	driver_msg.PROC_NR	= proc_nr;
	driver_msg.BUF		= buf;
	// assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
	// send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &driver_msg);

	/// replace the statement above.
	// disp_int(proc_nr);
	hd_rdwt(&driver_msg);
	return 0;
}

//added by xw, 18/8/27
PUBLIC int rw_sector_sched(int io_type, int dev, int pos, int bytes, int proc_nr, void* buf)
{
	MESSAGE driver_msg;
	
	driver_msg.type		= io_type;
	driver_msg.DEVICE	= MINOR(dev);

	driver_msg.POSITION	= pos;
	driver_msg.CNT		= bytes;	/// hu is: 512
	driver_msg.PROC_NR	= proc_nr;
	driver_msg.BUF		= buf;
	
	hd_rdwt_sched(&driver_msg);
	return 0;
}
//~xw

// added by zcr from chapter9/e/lib/open.c and modified it.

/*****************************************************************************
 *                                open
 *****************************************************************************/
/**
 * open/create a file.
 * 
 * @param pathname  The full path of the file to be opened/created.
 * @param flags     O_CREAT, O_RDWR, etc.
 * 
 * @return File descriptor if successful, otherwise -1.
 *****************************************************************************/
//open is a syscall interface now. added by xw, 18/6/18
// PUBLIC int open(const char *pathname, int flags)
PUBLIC int real_open(const char *pathname, int flags)
{
	//added by xw, 18/8/27
	MESSAGE fs_msg;

	fs_msg.type	= OPEN;
	fs_msg.PATHNAME	= (void*)pathname;
	fs_msg.FLAGS	= flags;
	fs_msg.NAME_LEN	= strlen(pathname);
	fs_msg.source = proc2pid(p_proc_current);

	//int fd = do_open();	//modified by xw, 18/8/27
	int fd = do_open(&fs_msg);

	// send_recv(BOTH, TASK_FS, &msg);
	// assert(msg.type == SYSCALL_RET);
	// return msg.FD;
	return fd;
}

/*****************************************************************************
 *                                do_open
 *****************************************************************************/
/**
 * Open a file and return the file descriptor.
 * 
 * @return File descriptor if successful, otherwise a negative error code.
 *****************************************************************************/
/// zcr modified.
PUBLIC int do_open(MESSAGE *fs_msg)
{
	/*caller_nr is the process number of the */
	int fd = -1;		/* return value */

	char pathname[MAX_PATH];

	/* get parameters from the message */
	int flags = fs_msg->FLAGS;	/* access mode */
	int name_len = fs_msg->NAME_LEN;	/* length of filename */
	int src = fs_msg->source;	/* caller proc nr. */

	/// zcr for debugging(the output is 0x1)
	// disp_str("src is: ");
	// disp_int(src);
	
	// assert(name_len < MAX_PATH);
	// phys_copy((void*)va2la(TASK_A, pathname), (void*)va2la(src, fs_msg.PATHNAME), name_len);
	phys_copy((void*)va2la(src, pathname), (void*)va2la(src, fs_msg->PATHNAME), name_len);
	pathname[name_len] = 0;

	/* find a free slot in PROCESS::filp[] */
	int i;
	for (i = 0; i < NR_FILES; i++) {
		if (p_proc_current->task.filp[i] == 0) {
			fd = i;
			break;
		}
	}
	if ((fd < 0) || (fd >= NR_FILES)) {
		// panic("filp[] is full (PID:%d)", proc2pid(p_proc_current));
		disp_str("filp[] is full (PID:");
		disp_int(proc2pid(p_proc_current));
		disp_str(")\n");
	}

	/* find a free slot in f_desc_table[] */
	for (i = 0; i < NR_FILE_DESC; i++)
		if (f_desc_table[i].fd_inode == 0)
			break;
	if (i >= NR_FILE_DESC) {
		// panic("f_desc_table[] is full (PID:%d)", proc2pid(p_proc_current));
		disp_str("f_desc_table[] is full (PID:");
		disp_int(proc2pid(p_proc_current));
		disp_str(")\n");
	}

	int inode_nr = search_file(pathname);

	/// zcr debug (output is 0x0, right.)
	// disp_str("the inode_nr result of search_file(): ");
	// disp_int(inode_nr);
	// disp_str("\n");

	struct inode * pin = 0;
	if (flags & O_CREAT) {
		if (inode_nr) {
			disp_str("file exists.\n");	/// zcr
			return -1;
		}
		else {
			pin = create_file(pathname, flags);
			/// zcr debugging
			// disp_str("create a new file done.\n");
		}
	}
	else {
		// assert(flags & O_RDWR);

		char filename[MAX_PATH];
		struct inode * dir_inode;
		if (strip_path(filename, pathname, &dir_inode) != 0)
			return -1;
		pin = get_inode_sched(dir_inode->i_dev, inode_nr);	//modified by xw, 18/8/28
		/// zcr 
		disp_str("get the i-node of a file already exists.\n");
	}
	/// zcr for debugging
	// disp_int(pin);

	if (pin) {
		/* connects proc with file_descriptor */
		p_proc_current->task.filp[fd] = &f_desc_table[i];

		/* connects file_descriptor with inode */
		f_desc_table[i].fd_inode = pin;

		f_desc_table[i].fd_mode = flags;
		/* f_desc_table[i].fd_cnt = 1; */
		f_desc_table[i].fd_pos = 0;

		int imode = pin->i_mode & I_TYPE_MASK;

		if (imode == I_CHAR_SPECIAL) {
			MESSAGE driver_msg;

			// driver_msg.type = DEV_OPEN;
			int dev = pin->i_start_sect;
			// driver_msg.DEVICE = MINOR(dev);
			// assert(MAJOR(dev) == 4);
			// assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);

			// send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &driver_msg);
			
			/// zcr added to replace the statement above
			hd_open(MINOR(dev));
		}
		else if (imode == I_DIRECTORY) {
			// assert(pin->i_num == ROOT_INODE);
			/// zcr
			if(pin->i_num != ROOT_INODE) {
				disp_str("Panic: pin->i_num != ROOT_INODE");
			}
		}
		else {
			// assert(pin->i_mode == I_REGULAR);
			// disp_str("REGULAR MODE  ");
			if(pin->i_mode != I_REGULAR) {
				/// zcr modified.
				disp_str("Panic: pin->i_mode != I_REGULAR");
			}
		}
	}
	else {
		return -1;
	}

	return fd;
}

/*****************************************************************************
 *                                create_file
 *****************************************************************************/
/**
 * Create a file and return it's inode ptr.
 *
 * @param[in] path   The full path of the new file
 * @param[in] flags  Attribiutes of the new file
 *
 * @return           Ptr to i-node of the new file if successful, otherwise 0.
 * 
 * @see open()
 * @see do_open()
 *****************************************************************************/
PRIVATE struct inode * create_file(char * path, int flags)
{
	/// zcr debug
	disp_str("path: ");
	disp_str(path);
	disp_str("  ");

	char filename[MAX_PATH];
	struct inode * dir_inode;

	if (strip_path(filename, path, &dir_inode) != 0)
		return 0;

	int inode_nr = alloc_imap_bit(dir_inode->i_dev);
	/// zcr debug(output is 0x1,wrong! should be 0x5!)
	disp_str("inode_nr: ");
	disp_int(inode_nr);
	disp_str("    ");

	int free_sect_nr = alloc_smap_bit(dir_inode->i_dev, NR_DEFAULT_FILE_SECTS);
	/// zcr debug(output is 0x10E)
	// disp_str("free_sect_nr: ");
	// disp_int(free_sect_nr);
	// disp_str("\n");

	struct inode *newino = new_inode(dir_inode->i_dev, inode_nr, free_sect_nr);

	new_dir_entry(dir_inode, newino->i_num, filename);

	return newino;
}

/// zcr copied from ch9/e/fs/misc.c

/*****************************************************************************
 *                                memcmp
 *****************************************************************************/
/**
 * Compare memory areas.
 * 
 * @param s1  The 1st area.
 * @param s2  The 2nd area.
 * @param n   The first n bytes will be compared.
 * 
 * @return  an integer less than, equal to, or greater than zero if the first
 *          n bytes of s1 is found, respectively, to be less than, to match,
 *          or  be greater than the first n bytes of s2.
 *****************************************************************************/
PUBLIC int memcmp(const void * s1, const void *s2, int n)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = (const char *)s1;
	const char * p2 = (const char *)s2;
	int i;
	for (i = 0; i < n; i++,p1++,p2++) {
		if (*p1 != *p2) {
			return (*p1 - *p2);
		}
	}
	return 0;
}

/*****************************************************************************
 *                                strcmp
 *****************************************************************************/
/**
 * Compare two strings.
 * 
 * @param s1  The 1st string.
 * @param s2  The 2nd string.
 * 
 * @return  an integer less than, equal to, or greater than zero if s1 (or the
 *          first n bytes thereof) is  found,  respectively,  to  be less than,
 *          to match, or be greater than s2.
 *****************************************************************************/
PUBLIC int strcmp(const char * s1, const char *s2)
{
	if ((s1 == 0) || (s2 == 0)) { /* for robustness */
		return (s1 - s2);
	}

	const char * p1 = s1;
	const char * p2 = s2;

	for (; *p1 && *p2; p1++,p2++) {
		if (*p1 != *p2) {
			break;
		}
	}

	return (*p1 - *p2);
}


/*****************************************************************************
 *                                search_file
 *****************************************************************************/
/**
 * Search the file and return the inode_nr.
 *
 * @param[in] path The full path of the file to search.
 * @return         Ptr to the i-node of the file if successful, otherwise zero.
 * 
 * @see open()
 * @see do_open()
 *****************************************************************************/
PUBLIC int search_file(char * path)
{
	int i, j;

	char filename[MAX_PATH];
	memset(filename, 0, MAX_FILENAME_LEN);
	struct inode * dir_inode;
	if (strip_path(filename, path, &dir_inode) != 0)
		return 0;

	if (filename[0] == 0)	/* path: "/" */
		return dir_inode->i_num;

	/**
	 * Search the dir for the file.
	 */
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries =
	  dir_inode->i_size / DIR_ENTRY_SIZE; /**
					       * including unused slots
					       * (the file has been deleted
					       * but the slot is still there)
					       */
	int m = 0;
	struct dir_entry * pde;
	for (i = 0; i < nr_dir_blks; i++) {
		RD_SECT_SCHED(dir_inode->i_dev, dir_blk0_nr + i);	//modified by xw, 18/8/27
		pde = (struct dir_entry *)fsbuf;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (memcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
				return pde->inode_nr;
			if (++m > nr_dir_entries)
				break;
		}
		if (m > nr_dir_entries) /* all entries have been iterated */
			break;
	}

	/* file not found */
	return 0;
}

/*****************************************************************************
 *                                strip_path
 *****************************************************************************/
/**
 * Get the basename from the fullpath.
 *
 * In Orange'S FS v1.0, all files are stored in the root directory.
 * There is no sub-folder thing.
 *
 * This routine should be called at the very beginning of file operations
 * such as open(), read() and write(). It accepts the full path and returns
 * two things: the basename and a ptr of the root dir's i-node.
 *
 * e.g. After stip_path(filename, "/blah", ppinode) finishes, we get:
 *      - filename: "blah"
 *      - *ppinode: root_inode
 *      - ret val:  0 (successful)
 *
 * Currently an acceptable pathname should begin with at most one `/'
 * preceding a filename.
 *
 * Filenames may contain any character except '/' and '\\0'.
 *
 * @param[out] filename The string for the result.
 * @param[in]  pathname The full pathname.
 * @param[out] ppinode  The ptr of the dir's inode will be stored here.
 * 
 * @return Zero if success, otherwise the pathname is not valid.
 *****************************************************************************/
PUBLIC int strip_path(char * filename, const char * pathname, struct inode** ppinode)
{
	const char * s = pathname;
	char * t = filename;

	if (s == 0)
		return -1;

	if (*s == '/')
		s++;

	while (*s) {		/* check each character */
		if (*s == '/')
			return -1;
		*t++ = *s++;
		/* if filename is too long, just truncate it */
		if (t - filename >= MAX_FILENAME_LEN)
			break;
	}
	*t = 0;

	*ppinode = root_inode;

	return 0;
}

/*****************************************************************************
 *                                read_super_block
 *****************************************************************************/
/**
 * <Ring 1> Read super block from the given device then write it into a free
 *          super_block[] slot.
 * 
 * @param dev  From which device the super block comes.
 *****************************************************************************/
PRIVATE void read_super_block(int dev)
{
	int i;
	MESSAGE driver_msg;

	driver_msg.type		= DEV_READ;
	driver_msg.DEVICE	= MINOR(dev);
	driver_msg.POSITION	= SECTOR_SIZE * 1;
	driver_msg.BUF		= fsbuf;
	driver_msg.CNT		= SECTOR_SIZE;
	driver_msg.PROC_NR	= proc2pid(p_proc_current);///TASK_A
	// assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
	// send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &driver_msg);
	/// zcr added
	// disp_str("In read_super_block()  before hd_rdwt\n");	/// just for debugging
	hd_rdwt(&driver_msg);
	// disp_str("In read_super_block()  after hd_rdwt\n");

	/* find a free slot in super_block[] */
	for (i = 0; i < NR_SUPER_BLOCK; i++)
		if (super_block[i].sb_dev == NO_DEV)
			break;
	if (i == NR_SUPER_BLOCK)
		disp_str("Panic: super_block slots used up");	/// zcr modified.

	// assert(i == 0); /* currently we use only the 1st slot */

	struct super_block * psb = (struct super_block *)fsbuf;

	super_block[i] = *psb;
	super_block[i].sb_dev = dev;
}


/*****************************************************************************
 *                                get_super_block
 *****************************************************************************/
/**
 * <Ring 1> Get the super block from super_block[].
 * 
 * @param dev Device nr.
 * 
 * @return Super block ptr.
 *****************************************************************************/
// PUBLIC struct super_block * get_super_block(int dev)
// {
// 	struct super_block * sb = super_block;
// 	for (; sb < &super_block[NR_SUPER_BLOCK]; sb++)
// 		if (sb->sb_dev == dev)
// 			return sb;

// 	disp_str("Panic: super block of devie ");
// 	disp_int(dev);
// 	disp_str(" not found.\n");

// 	return 0;
// }

/// zcr(using hu's method.)
PUBLIC struct super_block * get_super_block(int dev)
{
	struct super_block * sb = super_block;
	for (; sb < &super_block[NR_SUPER_BLOCK]; sb++){
		if (sb->sb_dev == dev)
			return sb;
	}
	return 0;
}



/*****************************************************************************
 *                                get_inode
 *****************************************************************************/
/**
 * <Ring 1> Get the inode ptr of given inode nr. A cache -- inode_table[] -- is
 * maintained to make things faster. If the inode requested is already there,
 * just return it. Otherwise the inode will be read from the disk.
 * 
 * @param dev Device nr.
 * @param num I-node nr.
 * 
 * @return The inode ptr requested.
 *****************************************************************************/
PUBLIC struct inode * get_inode(int dev, int num)
{
	if (num == 0)
		return 0;

	struct inode * p;
	struct inode * q = 0;
	for (p = &inode_table[0]; p < &inode_table[NR_INODE]; p++) {
		if (p->i_cnt) {	/* not a free slot */
			if ((p->i_dev == dev) && (p->i_num == num)) {
				/* this is the inode we want */
				p->i_cnt++;
				return p;
			}
		}
		else {		/* a free slot */
			if (!q) /* q hasn't been assigned yet */
				q = p; /* q <- the 1st free slot */
		}
	}

	if (!q)
		disp_str("Panic: the inode table is full");

	q->i_dev = dev;
	q->i_num = num;
	q->i_cnt = 1;

	struct super_block * sb = get_super_block(dev);
	int blk_nr = 1 + 1 + sb->nr_imap_sects + sb->nr_smap_sects + ((num - 1) / (SECTOR_SIZE / INODE_SIZE));
	RD_SECT(dev, blk_nr);
	struct inode * pinode =
		(struct inode*)((u8*)fsbuf +
				((num - 1 ) % (SECTOR_SIZE / INODE_SIZE))
				 * INODE_SIZE);
	q->i_mode = pinode->i_mode;
	q->i_size = pinode->i_size;
	q->i_start_sect = pinode->i_start_sect;
	q->i_nr_sects = pinode->i_nr_sects;
	return q;
}

//added by xw, 18/8/27
PUBLIC struct inode * get_inode_sched(int dev, int num)
{
	if (num == 0)
		return 0;

	struct inode * p;
	struct inode * q = 0;
	for (p = &inode_table[0]; p < &inode_table[NR_INODE]; p++) {
		if (p->i_cnt) {	/* not a free slot */
			if ((p->i_dev == dev) && (p->i_num == num)) {
				/* this is the inode we want */
				p->i_cnt++;
				return p;
			}
		}
		else {		/* a free slot */
			if (!q) /* q hasn't been assigned yet */
				q = p; /* q <- the 1st free slot */
		}
	}

	if (!q)
		disp_str("Panic: the inode table is full");

	q->i_dev = dev;
	q->i_num = num;
	q->i_cnt = 1;

	struct super_block * sb = get_super_block(dev);
	int blk_nr = 1 + 1 + sb->nr_imap_sects + sb->nr_smap_sects + ((num - 1) / (SECTOR_SIZE / INODE_SIZE));
	RD_SECT_SCHED(dev, blk_nr);
	struct inode * pinode =
		(struct inode*)((u8*)fsbuf +
				((num - 1 ) % (SECTOR_SIZE / INODE_SIZE))
				 * INODE_SIZE);
	q->i_mode = pinode->i_mode;
	q->i_size = pinode->i_size;
	q->i_start_sect = pinode->i_start_sect;
	q->i_nr_sects = pinode->i_nr_sects;
	return q;
}

/*****************************************************************************
 *                                put_inode
 *****************************************************************************/
/**
 * Decrease the reference nr of a slot in inode_table[]. When the nr reaches
 * zero, it means the inode is not used any more and can be overwritten by
 * a new inode.
 * 
 * @param pinode I-node ptr.
 *****************************************************************************/
PUBLIC void put_inode(struct inode * pinode)
{
	// assert(pinode->i_cnt > 0);
	pinode->i_cnt--;
}

/*****************************************************************************
 *                                sync_inode
 *****************************************************************************/
/**
 * <Ring 1> Write the inode back to the disk. Commonly invoked as soon as the
 *          inode is changed.
 * 
 * @param p I-node ptr.
 *****************************************************************************/
PUBLIC void sync_inode(struct inode * p)
{
	struct inode * pinode;
	struct super_block * sb = get_super_block(p->i_dev);
	int blk_nr = 1 + 1 + sb->nr_imap_sects + sb->nr_smap_sects + ((p->i_num - 1) / (SECTOR_SIZE / INODE_SIZE));
	RD_SECT_SCHED(p->i_dev, blk_nr);		//modified by xw, 18/8/28
	pinode = (struct inode*)((u8*)fsbuf +
				 (((p->i_num - 1) % (SECTOR_SIZE / INODE_SIZE))
				  * INODE_SIZE));
	pinode->i_mode = p->i_mode;
	pinode->i_size = p->i_size;
	pinode->i_start_sect = p->i_start_sect;
	pinode->i_nr_sects = p->i_nr_sects;
	WR_SECT_SCHED(p->i_dev, blk_nr);		//modified by xw, 18/8/28
}

/// added by zcr (from ch9/e/fs/open.c)
/*****************************************************************************
 *                                new_inode
 *****************************************************************************/
/**
 * Generate a new i-node and write it to disk.
 * 
 * @param dev  Home device of the i-node.
 * @param inode_nr  I-node nr.
 * @param start_sect  Start sector of the file pointed by the new i-node.
 * 
 * @return  Ptr of the new i-node.
 *****************************************************************************/
PRIVATE struct inode * new_inode(int dev, int inode_nr, int start_sect)
{
	struct inode * new_inode = get_inode_sched(dev, inode_nr);	//modified by xw, 18/8/28

	new_inode->i_mode = I_REGULAR;
	new_inode->i_size = 0;
	new_inode->i_start_sect = start_sect;
	new_inode->i_nr_sects = NR_DEFAULT_FILE_SECTS;

	new_inode->i_dev = dev;
	new_inode->i_cnt = 1;
	new_inode->i_num = inode_nr;

	/* write to the inode array */
	sync_inode(new_inode);

	return new_inode;
}

/*****************************************************************************
 *                                new_dir_entry
 *****************************************************************************/
/**
 * Write a new entry into the directory.
 * 
 * @param dir_inode  I-node of the directory.
 * @param inode_nr   I-node nr of the new file.
 * @param filename   Filename of the new file.
 *****************************************************************************/
PRIVATE void new_dir_entry(struct inode *dir_inode,int inode_nr,char *filename)
{
	/* write the dir_entry */
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE) / SECTOR_SIZE;
	int nr_dir_entries =
		dir_inode->i_size / DIR_ENTRY_SIZE; /**
						     * including unused slots
						     * (the file has been
						     * deleted but the slot
						     * is still there)
						     */
	int m = 0;
	struct dir_entry * pde;
	struct dir_entry * new_de = 0;

	int i, j;
	for (i = 0; i < nr_dir_blks; i++) {
		RD_SECT_SCHED(dir_inode->i_dev, dir_blk0_nr + i);	//modified by xw, 18/8/28

		pde = (struct dir_entry *)fsbuf;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (++m > nr_dir_entries)
				break;

			if (pde->inode_nr == 0) { /* it's a free slot */
				new_de = pde;
				break;
			}
		}
		if (m > nr_dir_entries ||/* all entries have been iterated or */
		    new_de)              /* free slot is found */
			break;
	}
	if (!new_de) { /* reached the end of the dir */
		new_de = pde;
		dir_inode->i_size += DIR_ENTRY_SIZE;
	}
	new_de->inode_nr = inode_nr;
	strcpy(new_de->name, filename);

	/* write dir block -- ROOT dir block */
	WR_SECT_SCHED(dir_inode->i_dev, dir_blk0_nr + i);		//modified by xw, 18/8/28

	/* update dir inode */
	sync_inode(dir_inode);
}


/*****************************************************************************
 *                                alloc_imap_bit
 *****************************************************************************/
/**
 * Allocate a bit in inode-map.
 * 
 * @param dev  In which device the inode-map is located.
 * 
 * @return  I-node nr.
 *****************************************************************************/
PRIVATE int alloc_imap_bit(int dev)
{
	/// zcr debug(output is 0x320, right)
	// disp_str("dev is: ");
	// disp_int(dev);
	// disp_str("  ");

	int inode_nr = 0;
	int i, j, k;

	int imap_blk0_nr = 1 + 1; /* 1 boot sector & 1 super block */
	struct super_block * sb = get_super_block(dev);

	/// zcr debug (output is 0x1, right.)
	// disp_str("How many the inode-map sectors: ");
	// disp_int(sb->nr_imap_sects);
	// disp_str("  ");

	for (i = 0; i < sb->nr_imap_sects; i++) {
		// RD_SECT(dev, imap_blk0_nr + i);		/// zcr: place the result in fsbuf?
		RD_SECT_SCHED(dev, imap_blk0_nr + i);	//modified by xw, 18/8/28
		/// zcr debug(output is 0x2, right.)
		// disp_str("imap_blk0_nr + i: ");
		// disp_int(imap_blk0_nr + i);
		// disp_str("  ");
		
		/// zcr debug
		// disp_str("fsbuf:");
		// for(int q = 0;q < 10;q++) {
		// 	disp_int(fsbuf[q]);
		// 	disp_str(" ");
		// }
		// disp_str("\n");

		for (j = 0; j < SECTOR_SIZE; j++) {
			/* skip `11111111' bytes */
			if (fsbuf[j] == 0xFF)
				continue;

			/* skip `1' bits */
			for (k = 0; ((fsbuf[j] >> k) & 1) != 0; k++) {}
			/// zcr debug(output is 1,wrong! should be 5)
			// disp_str("k is: ");
			// disp_int(k);
			// disp_str("  ");

			// disp_str("j is: ");
			// disp_int(j);
			// disp_str("  ");

			/* i: sector index; j: byte index; k: bit index */
			inode_nr = (i * SECTOR_SIZE + j) * 8 + k;
			fsbuf[j] |= (1 << k);

			/* write the bit to imap */
			WR_SECT_SCHED(dev, imap_blk0_nr + i);	//modified by xw, 18/8/28
			break;
		}

		return inode_nr;
	}

	/* no free bit in imap */
	disp_str("Panic: inode-map is probably full.\n");

	return 0;
}

/*****************************************************************************
 *                                alloc_smap_bit
 *****************************************************************************/
/**
 * Allocate a bit in sector-map.
 * 
 * @param dev  In which device the sector-map is located.
 * @param nr_sects_to_alloc  How many sectors are allocated.
 * 
 * @return  The 1st sector nr allocated.
 *****************************************************************************/
PRIVATE int alloc_smap_bit(int dev, int nr_sects_to_alloc)
{
	/* int nr_sects_to_alloc = NR_DEFAULT_FILE_SECTS; */

	int i; /* sector index */
	int j; /* byte index */
	int k; /* bit index */

	struct super_block * sb = get_super_block(dev);

	int smap_blk0_nr = 1 + 1 + sb->nr_imap_sects;
	int free_sect_nr = 0;

	for (i = 0; i < sb->nr_smap_sects; i++) { /* smap_blk0_nr + i :
						     current sect nr. */
		RD_SECT_SCHED(dev, smap_blk0_nr + i);	//modified by xw, 18/8/28

		/* byte offset in current sect */
		for (j = 0; j < SECTOR_SIZE && nr_sects_to_alloc > 0; j++) {
			k = 0;
			if (!free_sect_nr) {
				/* loop until a free bit is found */
				if (fsbuf[j] == 0xFF) continue;
				for (; ((fsbuf[j] >> k) & 1) != 0; k++) {}
				free_sect_nr = (i * SECTOR_SIZE + j) * 8 +
					k - 1 + sb->n_1st_sect;
			}

			for (; k < 8; k++) { /* repeat till enough bits are set */
				// assert(((fsbuf[j] >> k) & 1) == 0);
				fsbuf[j] |= (1 << k);
				if (--nr_sects_to_alloc == 0)
					break;
			}
		}

		if (free_sect_nr) /* free bit found, write the bits to smap */
			WR_SECT_SCHED(dev, smap_blk0_nr + i);	//modified by xw, 18/8/28

		if (nr_sects_to_alloc == 0)
			break;
	}

	// assert(nr_sects_to_alloc == 0);

	return free_sect_nr;
}

/// zcr added and modified from ch9/e/fs.open.c and close.c
/*****************************************************************************
 *                                close
 *****************************************************************************/
/**
 * Close a file descriptor.
 * 
 * @param fd  File descriptor.
 * 
 * @return Zero if successful, otherwise -1.
 *****************************************************************************/
//close is a syscall interface now. added by xw, 18/6/18
// PUBLIC int close(int fd)
PUBLIC int real_close(int fd)
{
	return do_close(fd);	// terrible(always returns 0)
}

/*****************************************************************************
 *                                do_close
 *****************************************************************************/
/**
 * Handle the message CLOSE.
 * 
 * @return Zero if success.
 *****************************************************************************/
PUBLIC int do_close(int fd)
{
	/// zcr debug
	// disp_str("hh1 ");
	// disp_int(fd);
	put_inode(p_proc_current->task.filp[fd]->fd_inode);
	// disp_str("hh2");
	p_proc_current->task.filp[fd]->fd_inode = 0;
	p_proc_current->task.filp[fd] = 0;

	return 0;
}

/// zcr copied from ch9/f/fs/read_write.c and modified it.

/*****************************************************************************
 *                                read
 *****************************************************************************/
/**
 * Read from a file descriptor.
 * 
 * @param fd     File descriptor.
 * @param buf    Buffer to accept the bytes read.
 * @param count  How many bytes to read.
 * 
 * @return  On success, the number of bytes read are returned.
 *          On error, -1 is returned.
 *****************************************************************************/
//read is a syscall interface now. added by xw, 18/6/18
// PUBLIC int read(int fd, void *buf, int count)
PUBLIC int real_read(int fd, void *buf, int count)
{
	//added by xw, 18/8/27
	MESSAGE fs_msg;
	PROCESS *p_proc_current;
	
	fs_msg.type = READ;
	fs_msg.FD   = fd;
	fs_msg.BUF  = buf;
	fs_msg.CNT  = count;

	// send_recv(BOTH, TASK_FS, &msg);
	do_rdwt(&fs_msg);

	return fs_msg.CNT;
}

/*****************************************************************************
 *                                write
 *****************************************************************************/
/**
 * Write to a file descriptor.
 * 
 * @param fd     File descriptor.
 * @param buf    Buffer including the bytes to write.
 * @param count  How many bytes to write.
 * 
 * @return  On success, the number of bytes written are returned.
 *          On error, -1 is returned.
 *****************************************************************************/
//write is a syscall interface now. added by xw, 18/6/18
// PUBLIC int write(int fd, const void *buf, int count)
PUBLIC int real_write(int fd, const void *buf, int count)
{
	//added by xw, 18/8/27
	MESSAGE fs_msg;
	
	fs_msg.type = WRITE;
	fs_msg.FD   = fd;
	fs_msg.BUF  = (void*)buf;
	fs_msg.CNT  = count;

	// send_recv(BOTH, TASK_FS, &msg);
	/// zcr added
	do_rdwt(&fs_msg);
	
	return fs_msg.CNT;
}


/*****************************************************************************
 *                                do_rdwt
 *****************************************************************************/
/**
 * Read/Write file and return byte count read/written.
 *
 * Sector map is not needed to update, since the sectors for the file have been
 * allocated and the bits are set when the file was created.
 * 
 * @return How many bytes have been read/written.
 *****************************************************************************/
PUBLIC int do_rdwt(MESSAGE *fs_msg)
{
	int fd = fs_msg->FD;	/**< file descriptor. */
	void * buf = fs_msg->BUF;/**< r/w buffer */
	int len = fs_msg->CNT;	/**< r/w bytes */

	int src = fs_msg->source;		/* caller proc nr. */

	if (!(p_proc_current->task.filp[fd]->fd_mode & O_RDWR))
		return -1;

	int pos = p_proc_current->task.filp[fd]->fd_pos;

	struct inode * pin = p_proc_current->task.filp[fd]->fd_inode;

	int imode = pin->i_mode & I_TYPE_MASK;

	if (imode == I_CHAR_SPECIAL) {
		int t = fs_msg->type == READ ? DEV_READ : DEV_WRITE;
		fs_msg->type = t;

		int dev = pin->i_start_sect;
		// assert(MAJOR(dev) == 4);
		/// zcr added
		if(MAJOR(dev) != 4) {
			disp_str("Error: MAJOR(dev) == 4\n");
		}

		fs_msg->DEVICE	= MINOR(dev);
		fs_msg->BUF	= buf;
		fs_msg->CNT	= len;
		fs_msg->PROC_NR	= src;
		// assert(dd_map[MAJOR(dev)].driver_nr != INVALID_DRIVER);
		// send_recv(BOTH, dd_map[MAJOR(dev)].driver_nr, &fs_msg);
		/// zcr added to replace the statement above
		hd_rdwt_sched(&fs_msg);		//modified by xw, 18/8/27
		// assert(fs_msg.CNT == len);

		return fs_msg->CNT;
	}
	else {
		// assert(pin->i_mode == I_REGULAR || pin->i_mode == I_DIRECTORY);
		// assert((fs_msg.type == READ) || (fs_msg.type == WRITE));

		int pos_end;
		if (fs_msg->type == READ)
			pos_end = min(pos + len, pin->i_size);
		else		/* WRITE */
			pos_end = min(pos + len, pin->i_nr_sects * SECTOR_SIZE);

		int off = pos % SECTOR_SIZE;
		int rw_sect_min = pin->i_start_sect + (pos>>SECTOR_SIZE_SHIFT);
		int rw_sect_max = pin->i_start_sect + (pos_end>>SECTOR_SIZE_SHIFT);

		int chunk = min(rw_sect_max - rw_sect_min + 1,
				FSBUF_SIZE >> SECTOR_SIZE_SHIFT);

		int bytes_rw = 0;
		int bytes_left = len;
		int i;
		for (i = rw_sect_min; i <= rw_sect_max; i += chunk) {
			/* read/write this amount of bytes every time */
			int bytes = min(bytes_left, chunk * SECTOR_SIZE - off);
			rw_sector_sched(DEV_READ,		//modified by xw, 18/8/27
				  pin->i_dev,
				  i * SECTOR_SIZE,
				  chunk * SECTOR_SIZE,
				  proc2pid(p_proc_current),	/// TASK_FS
				  fsbuf);

			if (fs_msg->type == READ) {
				phys_copy((void*)va2la(src, buf + bytes_rw),
					  (void*)va2la(proc2pid(p_proc_current), fsbuf + off),
					  bytes);
			}
			else {	/* WRITE */
				phys_copy((void*)va2la(proc2pid(p_proc_current), fsbuf + off),
					  (void*)va2la(src, buf + bytes_rw),
					  bytes);

				rw_sector_sched(DEV_WRITE,		//modified by xw, 18/8/27
					  pin->i_dev,
					  i * SECTOR_SIZE,
					  chunk * SECTOR_SIZE,
					  proc2pid(p_proc_current),
					  fsbuf);
			}
			off = 0;
			bytes_rw += bytes;
			p_proc_current->task.filp[fd]->fd_pos += bytes;
			bytes_left -= bytes;
		}

		if (p_proc_current->task.filp[fd]->fd_pos > pin->i_size) {
			/* update inode::size */
			pin->i_size = p_proc_current->task.filp[fd]->fd_pos;

			/* write the updated i-node back to disk */
			sync_inode(pin);
		}

		return bytes_rw;
	}
}

/// zcr copied from ch9/h/lib/unlink.c and modified it

/*****************************************************************************
 *                                unlink
 *****************************************************************************/
/**
 * Delete a file.
 * 
 * @param pathname  The full path of the file to delete.
 * 
 * @return Zero if successful, otherwise -1.
 *****************************************************************************/
//unlink is a syscall interface now. added by xw, 18/6/19
// PUBLIC int unlink(const char * pathname)
PUBLIC int real_unlink(const char * pathname)
{
	//added by xw, 18/8/27
	MESSAGE fs_msg;
	
	fs_msg.type   = UNLINK;

	fs_msg.PATHNAME	= (void*)pathname;
	fs_msg.NAME_LEN	= strlen(pathname);

	// send_recv(BOTH, TASK_FS, &msg);

	// return fs_msg.RETVAL;
	/// zcr added
	return do_unlink(&fs_msg);
}

/// zcr copied from the ch9/h/fs/link.c and modified it
/*****************************************************************************
 *                                do_unlink
 *****************************************************************************/
/**
 * Remove a file.
 *
 * @note We clear the i-node in inode_array[] although it is not really needed.
 *       We don't clear the data bytes so the file is recoverable.
 * 
 * @return On success, zero is returned.  On error, -1 is returned.
 *****************************************************************************/
PUBLIC int do_unlink(MESSAGE *fs_msg)
{
	char pathname[MAX_PATH];

	/* get parameters from the message */
	int name_len = fs_msg->NAME_LEN;	/* length of filename */
	int src = fs_msg->source;	/* caller proc nr. */
	// assert(name_len < MAX_PATH);
	phys_copy((void*)va2la(proc2pid(p_proc_current), pathname),
		  (void*)va2la(src, fs_msg->PATHNAME),
		  name_len);
	pathname[name_len] = 0;

	if (strcmp(pathname , "/") == 0) {
		/// zcr
		disp_str("FS:do_unlink():: cannot unlink the root\n");
		return -1;
	}

	int inode_nr = search_file(pathname);
	if (inode_nr == INVALID_INODE) {	/* file not found */
		/// zcr
		disp_str("FS::do_unlink():: search_file() returns invalid inode: ");
		disp_str(pathname);
		disp_str("\n");
		return -1;
	}

	char filename[MAX_PATH];
	struct inode * dir_inode;
	if (strip_path(filename, pathname, &dir_inode) != 0)
		return -1;

	struct inode * pin = get_inode_sched(dir_inode->i_dev, inode_nr);	//modified by xw, 18/8/28

	if (pin->i_mode != I_REGULAR) { /* can only remove regular files */
		// printl("cannot remove file %s, because it is not a regular file.\n", pathname);
		/// zcr
		disp_str("cannot remove file ");
		disp_str(pathname);
		disp_str(", because it is not a regular file.\n");
		return -1;
	}

	if (pin->i_cnt > 1) {	/* the file was opened */
		// printl("cannot remove file %s, because pin->i_cnt is %d.\n", pathname, pin->i_cnt);
		/// zcr
		disp_str("cannot remove file ");
		disp_str(pathname);
		disp_str(", because pin->i_cnt is ");
		disp_int(pin->i_cnt);
		disp_str("\n");
		return -1;
	}

	struct super_block * sb = get_super_block(pin->i_dev);

	/*************************/
	/* free the bit in i-map */
	/*************************/
	int byte_idx = inode_nr / 8;
	int bit_idx = inode_nr % 8;
	// assert(byte_idx < SECTOR_SIZE);	/* we have only one i-map sector */
	/* read sector 2 (skip bootsect and superblk): */
	RD_SECT_SCHED(pin->i_dev, 2);		//modified by xw, 18/8/28
	// assert(fsbuf[byte_idx % SECTOR_SIZE] & (1 << bit_idx));
	fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << bit_idx);
	WR_SECT_SCHED(pin->i_dev, 2);	//modified by xw, 18/8/28

	/**************************/
	/* free the bits in s-map */
	/**************************/
	/*
	 *           bit_idx: bit idx in the entire i-map
	 *     ... ____|____
	 *                  \        .-- byte_cnt: how many bytes between
	 *                   \      |              the first and last byte
	 *        +-+-+-+-+-+-+-+-+ V +-+-+-+-+-+-+-+-+
	 *    ... | | | | | |*|*|*|...|*|*|*|*| | | | |
	 *        +-+-+-+-+-+-+-+-+   +-+-+-+-+-+-+-+-+
	 *         0 1 2 3 4 5 6 7     0 1 2 3 4 5 6 7
	 *  ...__/
	 *      byte_idx: byte idx in the entire i-map
	 */
	bit_idx  = pin->i_start_sect - sb->n_1st_sect + 1;
	byte_idx = bit_idx / 8;
	int bits_left = pin->i_nr_sects;
	int byte_cnt = (bits_left - (8 - (bit_idx % 8))) / 8;

	/* current sector nr. */
	int s = 2  /* 2: bootsect + superblk */
		+ sb->nr_imap_sects + byte_idx / SECTOR_SIZE;

	RD_SECT_SCHED(pin->i_dev, s);		//modified by xw, 18/8/28

	int i;
	/* clear the first byte */
	for (i = bit_idx % 8; (i < 8) && bits_left; i++,bits_left--) {
		// assert((fsbuf[byte_idx % SECTOR_SIZE] >> i & 1) == 1);
		fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << i);
	}

	/* clear bytes from the second byte to the second to last */
	int k;
	i = (byte_idx % SECTOR_SIZE) + 1;	/* the second byte */
	for (k = 0; k < byte_cnt; k++,i++,bits_left-=8) {
		if (i == SECTOR_SIZE) {
			i = 0;
			WR_SECT_SCHED(pin->i_dev, s);		//modified by xw, 18/8/28
			RD_SECT_SCHED(pin->i_dev, ++s);		//modified by xw, 18/8/28
		}
		// assert(fsbuf[i] == 0xFF);
		fsbuf[i] = 0;
	}

	/* clear the last byte */
	if (i == SECTOR_SIZE) {
		i = 0;
		WR_SECT_SCHED(pin->i_dev, s);			//modified by xw, 18/8/28
		RD_SECT_SCHED(pin->i_dev, ++s);			//modified by xw, 18/8/28
	}
	unsigned char mask = ~((unsigned char)(~0) << bits_left);
	// assert((fsbuf[i] & mask) == mask);
	fsbuf[i] &= (~0) << bits_left;
	WR_SECT_SCHED(pin->i_dev, s);				//modified by xw, 18/8/28

	/***************************/
	/* clear the i-node itself */
	/***************************/
	pin->i_mode = 0;
	pin->i_size = 0;
	pin->i_start_sect = 0;
	pin->i_nr_sects = 0;
	sync_inode(pin);
	/* release slot in inode_table[] */
	put_inode(pin);

	/************************************************/
	/* set the inode-nr to 0 in the directory entry */
	/************************************************/
	int dir_blk0_nr = dir_inode->i_start_sect;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE) / SECTOR_SIZE;
	int nr_dir_entries =
		dir_inode->i_size / DIR_ENTRY_SIZE; /* including unused slots
						     * (the file has been
						     * deleted but the slot
						     * is still there)
						     */
	int m = 0;
	struct dir_entry * pde = 0;
	int flg = 0;
	int dir_size = 0;

	for (i = 0; i < nr_dir_blks; i++) {
		RD_SECT_SCHED(dir_inode->i_dev, dir_blk0_nr + i);	//modified by xw, 18/8/28

		pde = (struct dir_entry *)fsbuf;
		int j;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (++m > nr_dir_entries)
				break;

			if (pde->inode_nr == inode_nr) {
				/* pde->inode_nr = 0; */
				memset(pde, 0, DIR_ENTRY_SIZE);
				WR_SECT_SCHED(dir_inode->i_dev, dir_blk0_nr + i);	//modified by xw, 18/8/28
				flg = 1;
				break;
			}

			if (pde->inode_nr != INVALID_INODE)
				dir_size += DIR_ENTRY_SIZE;
		}

		if (m > nr_dir_entries || /* all entries have been iterated OR */
		    flg) /* file is found */
			break;
	}
	// assert(flg);
	if (m == nr_dir_entries) { /* the file is the last one in the dir */
		dir_inode->i_size = dir_size;
		sync_inode(dir_inode);
	}

	return 0;
}

/// zcr defined
//lseek is a syscall interface now. added by xw, 18/6/18
// PUBLIC int lseek(int fd, int offset, int whence)
PUBLIC int real_lseek(int fd, int offset, int whence)
{
	//added by xw, 18/8/27
	MESSAGE fs_msg;
	
	fs_msg.FD = fd;
	fs_msg.OFFSET = offset;
	fs_msg.WHENCE = whence;

	return do_lseek(&fs_msg);
}
/// zcr copied from ch9/j/fs/open.c
/*****************************************************************************
 *                                do_lseek
 *****************************************************************************/
/**
 * Handle the message LSEEK.
 * 
 * @return The new offset in bytes from the beginning of the file if successful,
 *         otherwise a negative number.
 *****************************************************************************/
PUBLIC int do_lseek(MESSAGE *fs_msg)
{
	int fd = fs_msg->FD;
	int off = fs_msg->OFFSET;
	int whence = fs_msg->WHENCE;

	int pos = p_proc_current->task.filp[fd]->fd_pos;
	int f_size = p_proc_current->task.filp[fd]->fd_inode->i_size;

	switch (whence) {
	case SEEK_SET:
		pos = off;
		break;
	case SEEK_CUR:
		pos += off;
		break;
	case SEEK_END:
		pos = f_size + off;
		break;
	default:
		return -1;
		break;
	}
	if ((pos > f_size) || (pos < 0)) {
		return -1;
	}
	p_proc_current->task.filp[fd]->fd_pos = pos;
	return pos;
}

//added by xw, 18/6/18
PUBLIC int sys_open(void *uesp)
{
	return real_open(get_arg(uesp, 1),
					 get_arg(uesp, 2));
}

PUBLIC int sys_close(void *uesp)
{
	return real_close(get_arg(uesp, 1));
}

PUBLIC int sys_read(void *uesp)
{
	return real_read(get_arg(uesp, 1),
					 get_arg(uesp, 2),
					 get_arg(uesp, 3));
}

PUBLIC int sys_write(void *uesp)
{
	return real_write(get_arg(uesp, 1),
					  get_arg(uesp, 2),
					  get_arg(uesp, 3));
}

PUBLIC int sys_lseek(void *uesp)
{
	return real_lseek(get_arg(uesp, 1),
					  get_arg(uesp, 2),
					  get_arg(uesp, 3));
}
//~xw, 18/6/18

//added by xw, 18/6/19
PUBLIC int sys_unlink(void *uesp)
{
	return real_unlink(get_arg(uesp, 1));
}
