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
#define _OPEN_SYS_ITOA_EXT

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
		char *breakpoints[100];
		int breakpointsAdded = 0;
	    while (WIFSTOPPED(wait_status)) {
	    	// Let user choose what to do next
	    	char options[256];  
	        printf("[>]");
	    	scanf("%s", options);

	    	// check rip if it matches a breakpoint 
	    	if (breakpointsAdded > 0){
	    		
				ptrace(PTRACE_GETREGS, pid, 0, &regs);
				for (int z=0; z < breakpointsAdded; z++)
					if (check_breakpoint(pid, breakpointsAdded, breakpoints[z], breakpoints)){
						printf("[x] Hit Breakpoint %s\n", breakpoints[z]);
						if (!stopped){
							temp_regs.rax = temp_regs.rax && 0xcc000000;
							ptrace(PTRACE_SETREGS, pid, 0, &temp_regs);
							stopped = true;
						}
						
					}
	    	}

	        if (strcmp(options, "cont")==0 || strcmp(options, "c")==0){	
				/* Make the child execute another instruction */
				
				if (ptrace(PTRACE_CONT, pid, NULL, NULL) < 0) {
		        // if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) < 0) {
		            perror("ptrace");
		            return -1;
		        }
		        /* Wait for child to stop on its next instruction */
	        	wait(&wait_status);
	        	if (stopped){
					ptrace(PTRACE_SETREGS, pid, 0, &regs);
				}
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
				show_registers(pid);
			}
			if (strcmp(options, "quit")==0 || strcmp(options, "q")==0){
				ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
				break;
			}
			if (strcmp(options, "break")==0){
				breakpointsAdded = request_breakpoint(breakpointsAdded, breakpoints);
				printf("[o] Added Break point %s [%d total]\n",
				 						breakpoints[breakpointsAdded-1], breakpointsAdded);
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