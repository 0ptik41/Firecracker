#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/reg.h> 
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <assert.h>
#include <wchar.h>
#define _OPEN_SYS_ITOA_EXT

struct PyReg {
	unsigned long rip;
	unsigned long rax;
	unsigned long rdx;
	unsigned long rcx;
	unsigned long rbp;
	unsigned long rdi;
	unsigned long rsp;
	unsigned long cs;
	unsigned long ss;
	unsigned long r09;
	unsigned long r10;
	unsigned long r11;
};

void show_registers(int pid){
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
	printf("RIP:\t0x%08llx\t", regs.rip);
	printf("RAX:\t0x%08llx\t", regs.rax);
	printf("R08:\t0x%08llx\n", regs.r8);
	printf("RCX:\t0x%08llx\t", regs.rcx);
	printf("RDX:\t0x%08llx\t", regs.rdx);
	printf("R09:\t0x%08llx\n", regs.r9);
	printf("RBX:\t0x%08llx\t", regs.rbx);
	printf("RSI:\t0x%08llx\t", regs.rsi);
	printf("R10:\t0x%08llx\n", regs.r10);
	printf("RDI:\t0x%08llx\t", regs.rdi);
	printf("RBP:\t0x%08llx\t", regs.rbp);
	printf("R11:\t0x%08llx\n", regs.r11);
	printf("SS:\t0x%08llx\t", regs.ss);
	printf("RSP:\t0x%08llx\t", regs.rsp);
	printf("CS:\t0x%08llx\n", regs.cs);
}

bool check_breakpoint(int pid, int n, char *point, char *breakpoints[]){
	bool isBreakpoint = false;
	for (int i =0; i < n; i++){
		if (strcmp(breakpoints[i], point)==0)
			isBreakpoint = true;
			break;
	}
	return isBreakpoint;
}

void set_registers(int pid, struct user_regs_struct new_regs){
	ptrace(PTRACE_SETREGS, pid, 0, &new_regs);
}

struct PyReg give_registers(int pid){
	struct PyReg regvals;
	struct user_regs_struct tmpregs;
	// Use Ptrace to get state of registers for given PID
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	// Copy these values into the struct for python
	regvals.rip = tmpregs.rip;	regvals.rax = tmpregs.rax;
	regvals.rdx = tmpregs.rdx;	regvals.rcx = tmpregs.rcx;
	regvals.rbp = tmpregs.rbp;	regvals.r09 = tmpregs.r9;
	regvals.r10 = tmpregs.r10;	regvals.r11 = tmpregs.r11;	
	regvals.rdi = tmpregs.rdi;
	regvals.cs = tmpregs.cs;
	regvals.ss = tmpregs.ss;
	return regvals;
}

/* Compile with: gcc -shared -fPIC -o dlib.so buggerlib.c */