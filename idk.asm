section		.text
global		start	


_start:
    ; Say Hello!
	mov 	edx, len
	mov		ecx, msg
	mov 	ebx, 1
	mov 	eax, 4
	int		0x80
	add		rdi, 0x1 	; count how many times through the loop 
	loop 	_start  	; Now Loop For Debugging the Debugger

_start.start:
	mov		edx, len
	mov 	ecx, msg
	mov 	ebx, 1
	mov 	eax, 4
	int 	0x80

section		.data
msg	db 0xa,' Hey! Gonna sit here for a while if you dont mind...', 0xa
len	equ $ - msg
