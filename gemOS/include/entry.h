#ifndef __ENTRY_H_
#define __ENTRY_H_

#include <types.h>
#include <context.h>

#define SYSCALL_EXIT      1
#define SYSCALL_GETPID    2
#define SYSCALL_ALARM     3
#define SYSCALL_SLEEP     4
#define SYSCALL_SIGNAL    5
#define SYSCALL_FORK      6

#define SYSCALL_STATS        11
#define SYSCALL_CONFIGURE    12
#define SYSCALL_PHYS_INFO    13 
#define SYSCALL_EXCEPTION_HACK  14


#define SYSCALL_EXPAND      20
#define SYSCALL_SHRINK      21
#define SYSCALL_MMAP        22 
#define SYSCALL_MUNMAP      23
#define SYSCALL_MPROTECT    24
#define SYSCALL_MREMAP      25

#define SYSCALL_OPEN        30
#define SYSCALL_READ        31
#define SYSCALL_WRITE       32
#define SYSCALL_PIPE        33
#define SYSCALL_DUP         34
#define SYSCALL_DUP2        35
#define SYSCALL_CLOSE       36
#define SYSCALL_LSEEK       37


extern long do_syscall(long syscall, u64 param1, u64 param2, u64 param3, u64 param4, struct user_regs *regs);

#endif //__ENTRY_H
