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
#define _OPEN_SYS_ITOA_EXT
#include <assert.h>
#include <wchar.h>


struct PyReg {
	const char* rip;
	const char* rax;
	const char* rdx;
	const char* rcx;
	const char* rbp;
	const char* cs;
	const char* ss;
	const char* r08;
	const char* r09;
	const char* r10;
	const char* r11;
};


void e(const char *p){
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
        printf("[!] Ptrace Error\n");
    }
	execl(p, "", NULL); 
}

void e1(const char *p, const char *a){
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
        printf("[!] Ptrace Error\n");
    }
	execl(p, a, NULL); 
}

void e2(const char *p,const char *a,const char *b){
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
        printf("[!] Ptrace Error\n");
    }
	execl(p, a, b, NULL); 
}

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
	printf("CS:\t0x%08llx\n", regs.cs);
	// printf("The child made a system call %lldn", regs.orig_rax);
}

struct PyReg give_registers(int pid){
	struct user_regs_struct regs;
	struct PyReg preg;
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
	preg.rip = (const char*)&regs.rip;
	return preg;
}


int request_breakpoint(int nbp, char *breakpoints[]){
	char address[1028];
	printf("Enter Address for breakpoint:\n");
	scanf("%s", address);
	breakpoints[nbp] = address; 
	return nbp + 1;
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

int start_debugee(const char* program){
	printf("[-] Launching %s\n", program);
	if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0){
        printf("[!] Ptrace Error\n");
    }
    char * const NUL = " &";
	return execv(program, &NUL);
}


int debugger(int nargin, const char * program, const char *args[]){
	// Setup Debugger Variables
	unsigned steps = 0;
	int wait_status;

	// create a progress to run the pogram for debugging
	int pid = fork();

	// check which process we are inside of
	if (pid == 0){
		// exec program
		switch (nargin){
			case 1:
				fprintf(stderr, "[!]Incorrect Usage\n");
				break;
			case 2:
				e(program);
				break;
			case 3:
				e1(program, args[2]);
				break;
			case 4:
				e2(program, args[2], args[3]);
				break;
			default:
				fprintf(stderr, "[!]Incorrect Usage\n");
				break;
		}

	} else if (pid >= 1){ // In parent, 
		wait(&wait_status);
		
		// probe process being inspected
		bool stopped = false;
		struct user_regs_struct temp_regs;
		struct user_regs_struct regs;
		ptrace(PTRACE_GETREGS, pid, 0, &regs);
		char *breakpoints[100];
		int breakpointsAdded = 0;
		int hits = 0;
	    while (WIFSTOPPED(wait_status)) {
	    	// Let user choose what to do next
	    	char options[256];  
	        printf("[>]");
	    	scanf("%s", options);

	        if (strcmp(options, "cont")==0 || strcmp(options, "c")==0){	
				if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
		            perror("ptrace");
		            return -1;
		        }
		        /* Wait for child to stop on its next instruction */
	        	wait(&wait_status);
	        	if (stopped){
					ptrace(PTRACE_SETREGS, pid, 0, &regs);
				}
			}

			if (strcmp(options, "break") == 0){
				ptrace(PTRACE_GETREGS, pid, 0, &temp_regs);
				temp_regs.rsp = temp_regs.rsp & 0xcc000000;
				ptrace(PTRACE_SETREGS, pid, 0, &temp_regs);
			}

			if (strcmp(options, "step") == 0){
				if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) < 0) {
		            perror("ptrace");
		            return -1;
		        }
		        /* Wait for child to stop on its next instruction */
	        	wait(&wait_status);	
			}
			if (strcmp(options, "show")==0){
				if (stopped)
					ptrace(PTRACE_SETREGS, pid, 0, &regs);
				show_registers(pid);
			}
			if (strcmp(options, "quit")==0 || strcmp(options, "q")==0){
				ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
				break;
			}
			steps++;

	        
		}
	}

	printf("[*] Completed. [%d instructions debugged]\n", steps);
	return 0;
}

int main(int argc, char const *argv[]){
	debugger(argc, argv[1], argv);
	return 0;
}