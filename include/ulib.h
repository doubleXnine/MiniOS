/********************************************
*用户库函数声明		add by visual 2016.5.16
*************************************************/

/*syscall.asm*/
int get_ticks();
int get_pid();					
void* kmalloc(int size);			
void* kmalloc_4k();			
void* malloc(int size);			
void* malloc_4k();				
int free(void *arg);				
int free_4k(void* AdddrLin);	
int fork();			
int pthread(void *arg);
//modified by xw, 18/5/30
void* udisp_int(int arg);
//void udisp_str(char* arg);
//void udisp_int(int arg);
void* udisp_str(char* arg);
//~xw

/*string.asm*/
void* memcpy(void* p_dst, void*  p_src, int size);//void* memcpy(void* es:p_dst, void* ds:p_src, int size);
void memset(void* p_dst, char ch, int size);
char* strcpy(char* p_dst, char* p_src);
