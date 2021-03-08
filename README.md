# Firecracker
An attempt at making my own little debugger for commandline (like gdb). It goes step by step through an (ELF only so far) executable,
allowing you to view important stack variables. 

![firecracker](https://raw.githubusercontent.com/0ptik41/Firecracker/main/torocracker.gif)

=========================================================================================

Below is a simple hello world .asm code, this is something we can actually step through reasonably.

section     .text
global      _start 
_start: 
    mov     edx,len   
    mov     ecx,msg   
    mov     ebx,1   
    mov     eax,4   
    int     0x80   
    mov     eax,1  
    int     0x80   
section     .data
msg     db  'Hello world',0xa  
len     equ $ - msg   

saved as hello.asm it can be compiled as follows.
nasm -f elf64 hello.asm # assemble the program  
ld -s -o hello hello.o # link the object file nasm produced into an executable file  
./hello # hello is an executable file

=========================================================================================

Running the "hello" assembly file using the compiled code, I can compare the values with what is seen running the same executable in GDB to be sure everything is working. 
![ex](https://raw.githubusercontent.com/0ptik41/Firecracker/main/ex.png)

=========================================================================================

# Next Steps 
Brandnew project so there's a lot more to do. I want to add a GUI component because there doesn't seem to be any GUI based
Linux Debugger that I know if. Reverse engineering frameworks like Ghidra might have support for something like that, but
I dont know how to do more than static analysis with it (that's also a *massive* project...).

================================|~ *Firecracker* ~ |======================================
