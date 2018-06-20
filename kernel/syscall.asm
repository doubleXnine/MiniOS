
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
_NR_get_pid       	equ 1 ;	//add by visual 2016.4.6
_NR_kmalloc       	equ 2 ;	//add by visual 2016.4.6
_NR_kmalloc_4k      equ 3 ;	//add by visual 2016.4.7
_NR_malloc      	equ 4 ;	//add by visual 2016.4.7
_NR_malloc_4k      	equ 5 ;	//add by visual 2016.4.7
_NR_free      		equ 6 ;	//add by visual 2016.4.7
_NR_free_4k      	equ 7 ;	//add by visual 2016.4.7
_NR_fork     		equ 8 ;	//add by visual 2016.4.8
_NR_pthread     	equ 9 ;	//add by visual 2016.4.11
_NR_udisp_int     	equ 10 ;	//add by visual 2016.5.16
_NR_udisp_str     	equ 11 ;	//add by visual 2016.5.16
_NR_exec     		equ 12 ;	//add by visual 2016.5.16
_NR_yield			equ 13 ;	//added by xw, 17/12
_NR_sleep			equ 14 ;	//added by xw, 17/12
_NR_print_E			equ 15 ;	//added by xw, 18/4/27
_NR_print_F			equ 16 ;	//added by xw, 18/4/27
_NR_open			equ 17 ;	//added by xw, 18/6/18
_NR_close			equ 18 ;	//added by xw, 18/6/18
_NR_read			equ 19 ;	//added by xw, 18/6/18
_NR_write			equ 20 ;	//added by xw, 18/6/18
_NR_lseek			equ 21 ;	//added by xw, 18/6/18
_NR_unlink			equ 22 ;	//added by xw, 18/6/18

INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global	get_pid		;		//add by visual 2016.4.6
global	kmalloc		;		//add by visual 2016.4.6
global	kmalloc_4k	;		//add by visual 2016.4.7
global	malloc		;		//add by visual 2016.4.7
global	malloc_4k	;		//add by visual 2016.4.7
global	free		;		//add by visual 2016.4.7
global	free_4k		;		//add by visual 2016.4.7
global	fork		;		//add by visual 2016.4.8
global	pthread		;		//add by visual 2016.4.11
global	udisp_int	;		//add by visual 2016.5.16
global	udisp_str	;		//add by visual 2016.5.16
global	exec		;		//add by visual 2016.5.16
global  yield		;		//added by xw
global  sleep		;		//added by xw
global	print_E		;		//added by xw
global	print_F		;		//added by xw
global	open		;		//added by xw, 18/6/18
global	close		;		//added by xw, 18/6/18
global	read		;		//added by xw, 18/6/18
global	write		;		//added by xw, 18/6/18
global	lseek		;		//added by xw, 18/6/18
global	unlink		;		//added by xw, 18/6/19

bits 32
[section .text]
; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              get_pid		//add by visual 2016.4.6
; ====================================================================
get_pid:
	mov	eax, _NR_get_pid
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              kmalloc		//add by visual 2016.4.6
; ====================================================================
kmalloc:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!
	mov	eax, _NR_kmalloc
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              kmalloc_4k		//add by visual 2016.4.7
; ====================================================================
kmalloc_4k:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!111
	mov	eax, _NR_kmalloc_4k
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              malloc		//add by visual 2016.4.7
; ====================================================================
malloc:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!111
	mov	eax, _NR_malloc
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              malloc_4k		//add by visual 2016.4.7
; ====================================================================
malloc_4k:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!111
	mov	eax, _NR_malloc_4k
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              free		//add by visual 2016.4.7
; ====================================================================
free:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!111
	mov	eax, _NR_free
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              free_4k		//add by visual 2016.4.7
; ====================================================================
free_4k:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!111
	mov	eax, _NR_free_4k
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              fork		//add by visual 2016.4.8
; ====================================================================
fork:
	;mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!说明:含有一个参数时,一定要这句,不含参数时,可以要这句,也可以不要这句,并不影响结果
	mov	eax, _NR_fork
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              pthread		//add by visual 2016.4.11
; ====================================================================
pthread:
	mov ebx,[esp+4] ; 将C函数调用时传来的参数放到ebx里!!说明:含有一个参数时,一定要这句,不含参数时,可以要这句,也可以不要这句,并不影响结果
	mov	eax, _NR_pthread
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              udisp_int		//add by visual 2016.5.16
; ====================================================================	
udisp_int:
	mov ebx,[esp+4]
	mov	eax, _NR_udisp_int
	int	INT_VECTOR_SYS_CALL
	ret
	
; ====================================================================
;                              udisp_str		//add by visual 2016.5.16
; ====================================================================	
udisp_str:
	mov ebx,[esp+4]
	mov	eax, _NR_udisp_str
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              exec		//add by visual 2016.5.16
; ====================================================================	
exec:
	mov ebx,[esp+4]
	mov	eax, _NR_exec
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              yield	//added by xw
; ====================================================================	
yield:
	mov ebx,[esp+4]
	mov	eax, _NR_yield
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              sleep	//added by xw
; ====================================================================	
sleep:
	mov ebx,[esp+4]
	mov	eax, _NR_sleep
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              print_E	//added by xw
; ====================================================================	
print_E:
	mov ebx,[esp+4]
	mov	eax, _NR_print_E
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              print_F	//added by xw
; ====================================================================	
print_F:
	mov ebx,[esp+4]
	mov	eax, _NR_print_F
	int	INT_VECTOR_SYS_CALL
	ret

; ====================================================================
;                              open		//added by xw, 18/6/18
; ====================================================================	
; open has more than one parameter, to pass them, we save the esp to ebx, 
; and ebx will be passed into kernel as usual. In kernel, we use the saved
; esp in user space to get the number of parameters and the values of them.
open:
	push 2			;the number of parameters
	mov ebx, esp
	mov	eax, _NR_open
	int	INT_VECTOR_SYS_CALL
	add esp, 4
	ret
	
; ====================================================================
;                              close	//added by xw, 18/6/18
; ====================================================================	
; close has only one parameter, but we can still use this method.
close:
	push 1			;the number of parameters
	mov ebx, esp
	mov	eax, _NR_close
	int	INT_VECTOR_SYS_CALL
	add esp, 4
	ret

; ====================================================================
;                              read		//added by xw, 18/6/18
; ====================================================================
read:
	push 3			;the number of parameters
	mov ebx, esp
	mov	eax, _NR_read
	int	INT_VECTOR_SYS_CALL
	add esp, 4
	ret

; ====================================================================
;                              write		//added by xw, 18/6/18
; ====================================================================
write:
	push 3			;the number of parameters
	mov ebx, esp
	mov	eax, _NR_write
	int	INT_VECTOR_SYS_CALL
	add esp, 4
	ret

; ====================================================================
;                              lseek		//added by xw, 18/6/18
; ====================================================================
lseek:
	push 3			;the number of parameters
	mov ebx, esp
	mov	eax, _NR_lseek
	int	INT_VECTOR_SYS_CALL
	add esp, 4
	ret
	
; ====================================================================
;                              unlink		//added by xw, 18/6/18
; ====================================================================
unlink:
	push 1			;the number of parameters
	mov ebx, esp
	mov	eax, _NR_unlink
	int	INT_VECTOR_SYS_CALL
	add esp, 4
	ret
