# LinuxKernel-Systemcalls-and-Userinterface

Goal
 In this assignment, you will implement a system call to the linux kernel and then you will build it. After building, you will call your own system call if your program is working in user space. The detailed scenario is given in the below. Implementation
 Details & Requirements 
Your code running into user space should run the system call is named as “processinfo”. Your program has been worked using four different usages without argument and with three arguments such as -p, -k and -all separately. 
Additionally, it may be taken as different time with the following parameters:

 ● without argument, it prints the right usage of this program. 
 
● “-all”, it lists all the processes and some information about them.

● “-p processID”, it outputs the given process ID and its command line.

 ● “-k processID”, it kills the given process ID.
 
 
 The sample executing formats and its functionality shortly. 
 
./processinfo.out >> program’s right usage 
./processinfo.out -all >> all processes some information
 ./processinfo.out -p 1425 >> process and its cmdline 
./processinfo.out -k 1425 >> kill process

 The sample outputs should be like that, if the program was run without argument, with -all, -p and -k arguments, respectively. 
without argument 
Right Usage: 
-all prints some information (process id and its argument/s) about all processes
 -p takes process id and prints the details of it
 -k takes process id 
 
with -all argument
 Process id and the argument list for the processes: 
uid    ppid   pid  name  state 

with -p argument 
Argument #[process_id]-Process Information:
 	pid commandline 
  
with -k argument
 Process xx was killed...
 
 
Linux, like most operating systems, interacts with hardware devices via modularized software components called device drivers. Under Linux, device drivers are part of the kernel and may be either linked statically into the kernel or loaded on demand as 

kernel modules. Device drivers run as part of the kernel and aren’t directly accessible to user processes. However, 
Linux provides a mechanism by which processes can communicate with a device driver—and through it with a hardware device—via file-like objects. These objects appear in the file system, and programs can open them, read from them, and write to them practically as if they were normal files. Using either Linux’s low-level I/O operations or the standard C library’s I/O operations, your programs can communicate with hardware devices through these file-like objects. 
Files in the /proc file system don’t correspond to actual files on a physical device. /proc contains virtual files, this is the reason it’s referred as a virtual file system. Instead, they are magic objects that behave like files but provide access to parameters, data structures, and statistics in the kernel. The “contents” of these files are not always fixed blocks of data, as ordinary file contents are. Instead, they are generated on the fly by the Linux kernel when you read from the file. 
/proc/[process_id]/status - The file provides much of the information in /proc/[pid]/stat and /proc/[pid]/statm in a format that's easier for humans to parse. > cat /proc/[process_id]/status 
/proc/[process_id]/cmdline - The cmdline entry contains the process argument list. The arguments are presented as a single character string, with arguments separated by NULs. NUL is the character with integer value 0. It’s different from NULL, which is a pointer with value 0. Most string functions expect that the entire character string is terminated with a single NUL and will not handle NULs embedded within strings, so you’ll have to handle the contents specially. 


Signals are mechanisms for communicating with and manipulating processes in Linux. A signal is a special message sent to a process. Signals are asynchronous; when a process receives a signal, it processes the signal immediately, without finishing the current function or even the current line of code. There are several dozen different signals, each with a different meaning. Each signal type is specified by its signal number, but in programs, you usually refer to a signal by its name. In Linux, these are defined in /usr/include/bits/signum.h. (You shouldn’t include this header file directly in your programs; instead, use .) For other information, you can read this documentation. 

Useful Lines 

● Useful headers: linux/kernel.h, linux/fs.h, linux/uaccess.h, linux/syscalls.h, linux/module.h, linux/string.h, linux/slab.h, asm/uaccess.h, linux/sched.h, asm/siginfo.h, linux/pid_namespace.h, linux/pid.h 

● kmalloc is the normal method of allocating memory for objects smaller than page size in the kernel.

● copy_from_user and copy_to_user are used to get and put single values (such as an int, char, or long) from and to userspace. 

● Each system call service routine C function definition is coded with the SYSCALL_DEFINE0 to SYSCALL_DEFINE6 macros. You must name the system call in the macro invocation. To define a system call that takes 2 parameters, you must use SYSCALL_DEFINE2, etc.
