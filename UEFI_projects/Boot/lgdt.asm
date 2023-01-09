org 8000h

use32
start:

; загрузка регистров GDTR
	lgdt fword[GDTR]

use64
; загрузка адреса PML4 в cr3
	xor rax, rax
	mov eax, dword[PML4]
	mov cr3, rax

; Переход к выполнению кода ядра
	exit:
		xor rax,rax
		mov eax, dword[c_code]
		jmp rax


label GDTR fword
db 2 dup(0)
db 4 dup(0)

PML4: 	db 4 dup(0)
c_code: db 4 dup(0)
