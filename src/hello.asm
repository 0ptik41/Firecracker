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
    mov	    rax, 0x60
    mov     dil, 0
    int     0x80

section     .data
msg     db  'Hello, friend.',0xa  
len     equ $ - msg
