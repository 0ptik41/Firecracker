#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/reg.h> 
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>


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

unsigned long get_rip(int pid){
	// Use Ptrace to get state of registers for given PID
	struct user_regs_struct tmpregs;
	ptrace(PTRACE_GETREGS, pid, 0, &tmpregs);
	return tmpregs.rip;	
}

void show_registers(int pid){
	struct user_regs_struct regs;
	ptrace(PTRACE_GETREGS, pid, 0, &regs);
	printf("RIP:\t0x%08llx\t", regs.rip);
	printf("RAX:\t0x%08llx\n", regs.rax);
	printf("RCX:\t0x%08llx\t", regs.rcx);
	printf("RDX:\t0x%08llx\n", regs.rdx);
	printf("RDI:\t0x%08llx\t", regs.rdi);
	printf("RBP:\t0x%08llx\n", regs.rbp);
	printf("SS:\t0x%08llx\t", regs.ss);
	printf("CS:\t0x%08llx\n", regs.cs);
}


int debugger(int nargin, const char * program, const char *args[]){
	// Setup Debugger Variables
	unsigned steps = 0;
	int wait_status;
	unsigned long *breakpoints[1000];
	int npoints = 0;
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
				printf("[*] Debugging %s\n", program);
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

	    while (WIFSTOPPED(wait_status)) {
	    	// check if  breakpoint was hit
	    	char options[256];
	        printf("[>]");
	    	scanf("%s", options);
	        if (strcmp(options, "step")==0){	
				/* Make the child execute another instruction */
		        if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) < 0) {
		            perror("ptrace");
		            return -1;
		        }
		        /* Wait for child to stop on its next instruction */
	        	wait(&wait_status);
			}
			if (strcmp(options, "show")==0){
				show_registers(pid);
			}

			if (strcmp(options ,"break")==0){
				scanf("%08lx",breakpoints[npoints]);
				printf("[*] Breakpoint added at %08lx [%d Breakpoint(s) total]\n", breakpoints[npoints], npoints);
				npoints = npoints + 1;
			}

			if (strcmp(options, "cont")==0){
				if(ptrace(PTRACE_CONT, pid, NULL, NULL) < 0){
					perror("ptrace");
				}
			}

			if (strcmp(options, "peek")==0){
				unsigned long addr;
				scanf("%08lx", &addr);
				unsigned data = ptrace(PTRACE_PEEKTEXT, pid, (void*)addr, 0);
				printf("0x%08lx: 0x%08x\n", addr, data);
			}


			if (strcmp(options, "quit")==0){
				ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
				break;
			}

			steps++;

	        
		}
	}

	printf("[*] Completed. [%d instructions debugged]\n", steps);
	return 0;
}

int main(int argc, char const *argv[])
{
	debugger(argc, argv[1], argv);
	return 0;
}