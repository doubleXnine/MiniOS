;;start.asm

extern main

bits 32

[section .text]

global _start

_start:
	push 	eax
	push	ecx
	call	main
	
	hlt
