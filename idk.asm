section		.text
global		start	


_start:
        ; Say Hello!
	mov 	edx, len
	mov	ecx, msg
	mov 	ebx, 1
	mov 	eax, 4
	int	0x80
	; Now Loop For Debugging the Debugger
	add	rdi, 0x1
	loop	_start

_start.start:
	mov	edx, len
	mov 	ecx, msg
	mov 	ebx, 1
	mov 	eax, 4
	int 	0x80

section		.data
msg	db 0xa,' Hey! Gonna sit here for a while if you dont mind...', 0xa
len	equ $ - msg
