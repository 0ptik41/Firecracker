#include <sys/types.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
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

unsigned long get_rax(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rax;
}

unsigned long get_rip(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rip;	
}

unsigned long get_rcx(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rcx;
}

unsigned long get_rdx(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rdx;
}

unsigned long get_rbx(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rbx;
}

unsigned long get_rsi(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rsi;
}

unsigned long get_rbp(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rbp;
}

unsigned long get_rdi(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rdi;
}

unsigned long get_rsp(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rsp;
}

bool continue_pid(int pid){
	bool completed = true;
	if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0){
		perror("ptrace");
		completed = false;
	}
	return completed;
}

void kill_pid(int pid){
	ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
}


bool step(int pid){
	bool stepped = true;
	if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) < 0){
		perror("ptrace");
		stepped = false;
	}
	return stepped;
}

void parent_wait(int wait_status){
	wait(&wait_status);
	return;
}


void pause_pid(int pid){
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	tmpregs.rsp = tmpregs.rsp & 0xcc0000000000;
	ptrace(PTRACE_SETREGS, pid, 0, &tmpregs);
   
}

void set_eperm_syscall(int pid){
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	tmpregs.rax = -EPERM; // Operation not permitted
    ptrace(PTRACE_SETREGS, pid, 0, &tmpregs);
}

void exec_next_syscall(int pid){
	/* Run system call and stop on exit */
    ptrace(PTRACE_SYSCALL, pid, 0, 0);
    waitpid(pid, 0, 0);
}

void set_invalid_syscall(int pid){
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	tmpregs.orig_rax = -1; // set to invalid syscall
 	ptrace(PTRACE_SETREGS, pid, 0, &tmpregs);
}

void traceme(){
	if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
        perror("[!] Ptrace Error\n");
    }
}

void show_instr_ptr(int pid){
	struct user_regs_struct regs;
	/* Obtain and show child's instruction pointer */
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
	printf("Child started. RIP = 0x%08x\n", regs.rip);
}

void get_addr_data(int pid, unsigned long *addr){
	unsigned data = ptrace(PTRACE_PEEKTEXT, pid, (void*)addr, 0);
	printf("0x%08lx: 0x%08lx\n", addr, data);

}


void set_breakpoint(int pid, unsigned long *addr){
	/* Write the trap instruction 'int 3' into the address */
	unsigned data_with_trap = (data & 0xFFFFFF00) | 0xCC;
	ptrace(PTRACE_POKETEXT, pid, (void*)addr, (void*)data_with_trap);
	/* See what's there again... */
	unsigned readback_data = ptrace(PTRACE_PEEKTEXT, pid, (void*)addr, 0);
	printf("x%08x: 0x%08x\n", addr, readback_data);
}

/* Compile with: gcc -shared -fPIC -o dlib.so buggerlib.c */