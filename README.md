# MiniOS简介
---
MiniOS是一个面向操作系统开发学习者的、微型的操作系统内核，可以运行在32位x86架构的CPU上。MiniOS专注于对操作系统开发中的核心概念和基础原理的学习与研究，并基于通用硬件对操作系统中的各基本子系统或模块进行实现。
流行的[Linux](https://github.com/torvalds/linux)、 [FreeBSD](https://github.com/freebsd/freebsd) 等操作系统内核固然很好，然而它们却并不适合内核开发的初学者。一方面，这些操作系统内核已经发展了很多年，积累了十分庞大的代码量（发布于2005年的Linux内核早期版本v2.6.12就已经有大约400万行代码），另一方面，因为应用在生产环境中的需要，这些内核代码中包含了大量和操作系统基本原理无关的细节，初学者很难抓到其中的要领。因此，从一个简单的、代码量较少的操作系统内核入手，使用较短的时间熟悉并掌握操作系统内核开发领域的核心概念和基础原理，等把这些基础性知识掌握到一定程度，再投身于Linux等实用内核的开发，对于内核初学者来说是一个比较现实可行的策略。即使不打算从事内核开发，通过一个易于入手的内核学习一些操作系统相关的基础知识，也会有利于写出更健壮、性能更好的应用程序。
查看MiniOS的[release_notes](https://github.com/doubleXnine/MiniOS/blob/master/release_notes.txt) 可了解MiniOS的当前开发进展。

# MiniOS开发工具
---
MiniOS主要基于C语言和x86汇编语言开发，使用的开发工具包括：

* 汇编器[nasm](https://www.nasm.us/)
* C语言编译器gcc
* GNU二进制工具集[Binutils](http://www.gnu.org/software/binutils/)
* 项目构建工具make
* 调试器gdb

其中，Binutils是一套对二进制文件进行操作的工具集，包括创建静态库的工具ar，从二进制文件中去除符号表以减小文件体积的工具strip等。

# 运行MiniOS
---
MiniOS当前从软盘中启动，启动流程为：
1. BIOS自检完毕后从软盘引导扇区中加载引导程序（boot.bin）至内存，并将控制权交给引导程序。
2. 引导程序从软盘中读取加载器（loader.bin）至内存，并将控制器交给加载器。
3. 加载器运行时会从软盘中读取MiniOS内核（kernel.bin）至内存，然后从CPU的实模式进入保护模式，并将控制权交给内核。
4. MiniOS开始运行。

由于MiniOS是一个面向学习者的操作系统内核，因此目前主要运行在虚拟机中，可选的虚拟机有[Bochs](http://bochs.sourceforge.net/)和[Qemu](https://www.qemu.org/)。

**在Bochs中运行MiniOS**
1. 安装Bochs，在Ubuntu系统下可以直接执行命令`sudo apt-get install bochs`进行安装，也可以先下载Bochs的源码再进行编译安装，通过源码进行安装可以选择想要的Bochs版本。
2. 进入MiniOS源目录，执行`tar zxvf misc/80m.img.tar.gz .`，从硬盘镜像压缩包中解压出硬盘镜像。
3. 在当前目录下执行`bochs`命令启动Bochs虚拟机，Bochs首先会从bochsrc文件中读取配置信息，然后对Bochs给出的运行提示信息进行确认便可让MiniOS在Bochs内运行。

**在Qemu中运行MiniOS**
1. 按照Qemu，在Ubuntu系统下可以直接执行命令`sudo apt-get install qemu-system-x86`进行按照，也可以下载Qemu的源代码进行编译安装。
2. 进入MiniOS源目录，执行`tar zxvf misc/80m.img.tar.gz .`，从硬盘镜像压缩包中解压出硬盘镜像。
3. 在当前目录下执行`./launch-qemu.sh`命令启动Qemu虚拟机，之后MiniOS将直接在Qemu内开始运行。Qemu虚拟机没有使用像bochsrc一样的配置文件，配置信息是通过命令行选项指定的，脚本launch-qemu.sh中包含了当前使用的配置选项。

# 调试MiniOS
通过使用Bochs或Qemu中自带的调试功能可以对MiniOS进行汇编语言级的调试，但由于汇编程序比较冗长且难以阅读，这种调试方式使用起来不太方便。幸运的是，Bochs和Qemu中都内置了gdb支持，通过和gdb提供的远程调试功能配合，可以对MiniOS进行C源码级的调试。

**使用Bochs+gdb调试MiniOS**
1. 从源代码编译安装Bochs，并在编译时打开gdb支持选项。然后在Bochs配置文件中添加gdb配置信息，MiniOS源目录下的bochsrc-gdb文件中已经包含了所需的配置选项。
2. 在MiniOS源目录下执行`./launch-bochs-gdb.sh`，所运行的shell脚本会在一个新的终端窗口中运行gdb，并加载debug版的内核二进制文件。
3. 在gdb命令界面执行命令`target remote :2345`和Bochs建立连接。
4. 用gdb像调试本地程序一样对MiniOS进行调试。

**使用Qemu+gdb调试MiniOS**
1. 在启动Qemu时添加命令行选项以启用gdb支持，MiniOS源目录下的脚本文件launch-qemu-gdb.sh中已经添加了所需的配置选项。
2. 在MiniOS源目录下执行`./launch-bochs-gdb.sh`，所运行的shell脚本会在一个新的终端窗口中运行gdb，并加载debug版的内核二进制文件。
3. 在gdb命令界面执行命令`target remote :1234`和Qemu建立连接。
4. 用gdb像调试本地程序一样对MiniOS进行调试。

# 常用MiniOS构建选项
```
# 编译MiniOS内核和用户程序init，并写入到软盘镜像a.img中
make image
# 清除所有.o目标文件
make clean
# 清除所有.o目标文件和可执行文件
make realclean
```

# 参考资料
* [Orange's](https://github.com/yyu/Oranges) ，由于渊开发的一个微型操作系统，在《一个操作系统的实现》这本书中讲述了Orange's的开发过程。MiniOS是基于Orange's进行开发的。
* [xv6](https://pdos.csail.mit.edu/6.828/2014/xv6.html) ， 由MIT开发的一个用于教学的微型操作系统，xv6由Unix V6改写而来，被应用在MIT的操作系统课程6.828: Operating System Engineering中。
* [Minix](http://www.minix3.org/) ，最初由Andrew S. Tanenbaum教授开发的一个微内核操作系统，Linus在开发早期的Linux的时候从Minix处继承了很多特性，于渊在开发Orange's的时候也多次借鉴了Minix。