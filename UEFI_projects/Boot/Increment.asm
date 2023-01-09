org 10000h
use16

; mov ax, 300h
; mov ds, ax
	while_true:
		inc word[ds:300h]
		jmp dword while_true
