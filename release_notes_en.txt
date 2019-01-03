***************************************************************************
MiniOS v1.1.1  Date:18/4/27

********
Description:
--some bugs fix and improvements.

********
Features added:
--

********
Bugs fix and improvements:
--Before a syscall returns to user space, call sched so there will be more
  chance to run a new process.
--Add p_proc_next in schedule() and sched, we only use p_proc_current
  before.
--Add new file ktest.c, where we put features test.
--Fix a bug in the syscall yield(), the ticks value of the relevant process
  should be assigned to 0, not just decreases by 1.

***************************************************************************
MiniOS v1.1  Date:18/4/21

********
Description:
--Interrupt handling and processes' giving up CPU voluntarily
  can work together in a better way now.

********
Features added:
--At MiniOS v1.0, we add a context in processes' kerne stack to
  support giving up CPU voluntarily. And at this version, 
  we extend this feature to interrupt handling, which the context struct
  will also be used in.
--sched()
  The sched() function is written in asm and existing in kernel.asm.
  It's the entry point of process switching. As we mentioned before, 
  interrupt handling and processes' giving up CPU can work together,
  in fact, it's implemented by both invoking sched().
  What sched() does can simply be described as saving current process's 
  context, invoking schedule(), renewing the new chosen process' executing
  environment and then, switching to the new process's kernel stack 
  and restoring it's context.
  sched() can be invoked only in kernel space, and naturally it always 
  returns back to kernel space.
--schedule()
  The schedule() function is written in C and existing in proc.c.
  It's the actually scheduler and determines which process will luckily
  get the CPU.
--With some modification of affectded code, the new procedure of 
  process switching can work with other parts correctly.
  You can use fork() and pthread() normally, and Kernel Preemption is
  still there.

********
Bugs fixed:
--With a modification of processes's initial ticks, the first process in
  the the proc_table outputs first as we want.
  Before the modification, each process only has 1 ticks in hand, and when our
  first process begin running, a clock-interruption will happen immediately.
  Then the second process in proc_table will be chosen and enjoy outputing
  first.
--k_reenter is used to handle interrupt nesting now, its value is increased
  only when an interrupt happens.

********************************************************************
MiniOS v1.0  Date:18/3/18

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

Description:
Firstly, it's my great pleasure to acknowledge the contributions of some people.
Thanks go to Forrest Yu, the author of Orange's, Mr Gu, the instructor of the current
OS-development project, and Lei Tao(visual), the author of 21pthread based on Orange's.

-My Bochs is v2.6.9, if yours isn't this version, may you need mofify the bochsrc.
-There is a "bochsrc.backup" which suits a former-version Bochs.
-It's assumed that you use a "x86_64" Linux, or you should rename the "Makefile.i386"
to "Makefile" before you run "make image".
-Now the os allows syscall to re-enter, in other words, you needn't wait a syscall ending
before you can run another. It's also called Kernel Preemption, in contrast to 
User Preemption.
-However, syscall fork and pthread can't work normally now.



