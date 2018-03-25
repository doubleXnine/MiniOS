********************************************************************
MiniOS v1.0  
Date:18/3/18  Author:xw  Contact:dongxuwei@163.com

********
Description:
--Use a new OS name, it's named MiniOS now.

********
Features added:
--At this version, a process can give up CPU on its own. 
  Therefore, some system-call, such as read, can block 
  itself when it waits the needed data.
--Add system call: yield
  yield() simply decreases the current process's ticks value,
  and calls save_context, which saves the current environment
  and switches to a new process.
--Add system call: sleep(n)
  When calling sleep(n), the current process will sleep n ticks.
  Whenever a clock interruption comes, the handler will wakeup 
  the processes sleeping on ticks, using a kernel function
  sys_wakeup.
  Note that there is no wakeup system call now!

********
Bugs fixed:
--

********************************************************************
MiniOS v0.9
Version:21pthread_3  Date:17/12/13
Author:xw  Contact:dongxuwei@163.com

Description:
-Fortunately, fork and pthread work normally now!
-What's more, fix the bug in function "disp_str" in klib.asm partially, the os can run
a long time now and won't generate a #GP fault, which caused by video-memory overflow.
-Attention! the stuct STACK_FRAME, which saves the registers when an interruption or
a syscall occurs before, is discarded. When you want to access the saved registers
in your kernel code, use a pointer plus a offset instead. The offset of each register
has been defined in proc.h.

********************************************************************
Version:21pthread_2  Date:17/12/11
Author:xw  Contact:dongxuwei@163.com

Description:
Firstly, it's my great pleasure to acknowledge the contributions of some people.
Thanks go to Forrest Yu, the author of Orange's, Mr Gu, the instructor of the current
OS-development project, and Lei Tao(visual), the author of 21pthread based on Orange's.

-My Bochs is v2.6.9, if yours isn't this version, may you need mofify the bochsrc.
-There is a "bochsrc.backup" which suits a former-version Bochs.
-It's assumed that you use a "x86_64" Linux, or you should rename the "Makefile.i386"
to "Makefile" before you run "make image".
-Now the os allows syscall to re-enter, in other words, you needn't wait a syscall ending
before you can run another.
-However, syscall fork and pthread can't work normally now.



