section .data
    string1 db  0xa, "  Hello Debugger! Just gonna sit here a while :D", 0xa, 0xa, 0

section .text
    global _start

    _start:
        ; calculate the length of string
        mov     rdi, string1        ; string1 to destination index
        xor     rcx, rcx            ; zero rcx
        not     rcx                 ; set rcx = -1
        xor     al,al               ; zero the al register (initialize to NUL)
        cld                         ; clear the direction flag
        repnz   scasb               ; get the string length (dec rcx through NUL)
        not     rcx                 ; rev all bits of negative results in absolute value
        dec     rcx                 ; -1 to skip the null-terminator, rcx contains length
        mov     rdx, rcx            ; put length in rdx
        ; write string to stdout
        mov     rsi, string1        ; string1 to source index
        mov     rax, 1              ; set write to command
        mov     rdi,rax             ; set destination index to rax (stdout)
        syscall                     ; call kernel
        ; exit 
        xor     rdi,rdi             ; zero rdi (rdi hold return value)
        mov     rax, 0x3c           ; set syscall number to 60 (0x3c hex)
        syscall                     ; call kernel
    
; Compile/Link
;
; nasm -f elf64 -o hello-stack_64.o hello-stack_64.asm
; ld  -o hello-stack_64 hello-stack_64.o
