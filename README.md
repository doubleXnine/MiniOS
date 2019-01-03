## MiniOS 简介
MiniOS 是一个面向操作系统开发学习者的、微型的操作系统内核，可以运行在32位 x86 架构的 CPU 上。MiniOS 专注于对操作系统开发中的核心概念和基础原理的学习与研究，并基于通用硬件对操作系统中的各基本子系统或模块进行实现。

流行的 [Linux](https://github.com/torvalds/linux) 、 [FreeBSD](https://github.com/freebsd/freebsd) 等操作系统内核固然很好，然而它们却并不适合内核开发的初学者。一方面，这些操作系统内核已经发展了很多年，积累了十分庞大的代码量（发布于 2005 年的 Linux 内核早期版本 v2.6.12 就已经有大约 400 万行代码），另一方面，因为应用在生产环境中的需要，这些内核代码中包含了大量和操作系统基本原理无关的细节，初学者很难抓到其中的要领。因此，从一个简单的、代码量较少的操作系统内核入手，使用较短的时间熟悉并掌握操作系统内核开发领域的核心概念和基础原理，等把这些基础性知识掌握到一定程度，再投身于 Linux 等实用内核的开发，对于内核初学者来说是一个比较现实可行的策略。即使不打算从事内核开发，通过一个易于入手的内核学习一些操作系统相关的基础知识，也会有利于写出更健壮、性能更好的应用程序。

* MiniOS 的GitHub仓库地址是 [https://github.com/doubleXnine/MiniOS](https://github.com/doubleXnine/MiniOS) 
* 查看 MiniOS 的 [release_notes](https://github.com/doubleXnine/MiniOS/blob/master/release_notes.txt) 可了解 MiniOS 的当前开发进展。

---

## MiniOS 开发工具
MiniOS 主要基于 C 语言和 x86 汇编语言开发，使用的开发工具包括：

* 汇编器 [nasm](https://www.nasm.us/) 
* C 语言编译器 gcc
* GNU 二进制工具集 [Binutils](http://www.gnu.org/software/binutils/) 
* 项目构建工具 make
* 调试器 gdb

其中，Binutils 是一套对二进制文件进行操作的工具集，包括创建静态库的工具 ar，从二进制文件中去除符号表以减小文件体积的工具 strip 等。


## 运行 MiniOS
MiniOS 当前从软盘中启动，启动流程为：
1. BIOS 自检完毕后从软盘引导扇区中加载引导程序（boot.bin）至内存，并将控制权交给引导程序。
2. 引导程序从软盘中读取加载器（loader.bin）至内存，并将控制器交给加载器。
3. 加载器运行时会从软盘中读取 MiniOS 内核（kernel.bin）至内存，然后从 CPU 的实模式进入保护模式，并将控制权交给内核。
4. MiniOS 开始运行。

由于 MiniOS 是一个面向学习者的操作系统内核，因此目前主要运行在虚拟机中，可选的虚拟机有 [Bochs](http://bochs.sourceforge.net/) 和 [Qemu](https://www.qemu.org/) 。

**在 Bochs 中运行 MiniOS**
1. 安装 Bochs，在 Ubuntu 系统下可以直接执行命令 `sudo apt-get install bochs` 进行安装，也可以先下载 Bochs 的源码再进行编译安装，通过源码进行安装可以选择想要的 Bochs 版本。
2. 进入 MiniOS 源目录，执行 `tar zxvf misc/80m.img.tar.gz .` ，从硬盘镜像压缩包中解压出硬盘镜像。
3. 在当前目录下执行 `bochs` 命令启动 Bochs 虚拟机，Bochs 首先会从 bochsrc 文件中读取配置信息，然后对 Bochs 给出的运行提示信息进行确认便可让MiniOS 在 Bochs 内运行。

**在 Qemu 中运行 MiniOS**
1. 安装 Qemu，在 Ubuntu 系统下可以直接执行命令 `sudo apt-get install qemu-system-x86` 进行安装，也可以下载 Qemu 的源代码进行编译安装。
2. 进入 MiniOS 源目录，执行 `tar zxvf misc/80m.img.tar.gz .` ，从硬盘镜像压缩包中解压出硬盘镜像。
3. 在当前目录下执行 `./launch-qemu.sh` 命令启动 Qemu 虚拟机，之后 MiniOS 将直接在 Qemu 内开始运行。Qemu 虚拟机没有使用像 bochsrc 一样的配置文件，配置信息是通过命令行选项指定的，脚本 launch-qemu.sh 中包含了当前使用的配置选项。

---

## 调试 MiniOS
通过使用 Bochs 或 Qemu 中自带的调试功能可以对 MiniOS 进行汇编语言级的调试，但由于汇编程序比较冗长且难以阅读，这种调试方式使用起来不太方便。幸运的是，Bochs 和 Qemu 中都内置了 gdb 支持，通过和 gdb 提供的远程调试功能配合，可以对 MiniOS 进行 C 源码级的调试。

**使用 Bochs+gdb 调试 MiniOS**
1. 从源代码编译安装 Bochs，并在编译时打开 gdb 支持选项。然后在 Bochs 配置文件中添加 gdb 配置信息，MiniOS 源目录下的 bochsrc-gdb 文件中已经包含了所需的配置选项。
2. 在 MiniOS 源目录下执行 `./launch-bochs-gdb.sh` ，所运行的 shell 脚本会在一个新的终端窗口中运行 gdb，并加载 debug 版的内核二进制文件。
3. 在 gdb 命令界面执行命令 `target remote :2345` 和 Bochs 建立连接。
4. 用 gdb 像调试本地程序一样对 MiniOS 进行调试。

**使用 Qemu+gdb 调试 MiniOS**
1. 在启动 Qemu 时添加命令行选项以启用 gdb 支持，MiniOS 源目录下的脚本文件 launch-qemu-gdb.sh 中已经添加了所需的配置选项。
2. 在 MiniOS 源目录下执行 `./launch-bochs-gdb.sh` ，所运行的 shell 脚本会在一个新的终端窗口中运行 gdb ，并加载 debug 版的内核二进制文件。
3. 在 gdb 命令界面执行命令 `target remote :1234` 和 Qemu 建立连接。
4. 用 gdb 像调试本地程序一样对 MiniOS 进行调试。

---

## 常用 MiniOS 构建选项
```
# 编译 MiniOS 内核和用户程序 init，并写入到软盘镜像 a.img 中
make image
# 清除所有 .o 目标文件
make clean
# 清除所有 .o 目标文件和可执行文件
make realclean
```

---

## 参考资料
* [Orange's](https://github.com/yyu/Oranges) ，由于渊开发的一个微型操作系统，在《一个操作系统的实现》这本书中讲述了 Orange's 的开发过程。MiniOS 是基于 Orange's 进行开发的。
* [xv6](https://pdos.csail.mit.edu/6.828/2014/xv6.html) ， 由 MIT 开发的一个用于教学的微型操作系统，xv6 由 Unix V6 改写而来，被应用在 MIT 的操作系统课程 6.828: Operating System Engineering 中。
* [Minix](http://www.minix3.org/) ，最初由 Andrew S. Tanenbaum 教授开发的一个微内核操作系统，Linus 在开发早期的 Linux 的时候从 Minix 处继承了很多特性，于渊在开发 Orange's 的时候也多次借鉴了 Minix 中的做法。
