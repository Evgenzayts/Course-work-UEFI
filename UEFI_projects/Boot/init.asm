org 10000h
use16

STACK 				equ 200000h
selektor_LM_CODE	equ 8 	; LM - Long Mode
selektor_LM_DATA 	equ 10h ; PM - Protected Mode
selektor_PM_CODE	equ 18h
selektor_PM_DATA	equ 20h

start:
	
	xor eax, eax
	mov eax, Long_mode
	mov dword[phys_addr], eax

	mov eax, GDT
	mov	dword[GDTR + 2], eax
	lgdt fword[GDTR]

; открытие линии A20:
	in	al, 92h
	or	al, 2
	out	92h, al
	
; запрет прерываний
	cli
	in	al, 70h
	or	al, 80h
	out	70h, al
	
; переключение в защищенный режим
	mov	eax, cr0
	or	al, 1
	mov	cr0, eax

jmp far fword[PM_addr] 
PM_addr:
	dd Protected_mode
	dw selektor_PM_CODE

Protected_mode:
	use32

	mov ax, selektor_PM_DATA
	mov ds, ax
	mov ax, selektor_LM_DATA
	mov es, ax
	mov esp, STACK

	; включение расширения PAE
	mov	eax, cr4
	or	eax, 1 shl 5
	mov	cr4, eax

	; очистка памяти под разметку
	mov edi, 170000h
	mov ecx, 1000h
	xor eax, eax
	rep stosd

	; PML4
	mov edi, 170000h
	mov dword[es:edi], 171007h

	; Каталог страниц
	mov edi, 171000h
	mov ecx, 2
	mov eax, 172007h

	PDPTE:
		stosd
		mov dword[es:edi], 0
		add edi, 4
		add eax, 1000h
		loop PDPTE

	; Таблицы страниц
	mov edi, 172000h
	mov ecx, 1024
	mov eax, 0 + 87h

	PDE:
		stosd
		mov dword[es:edi], 0
		add edi, 4
		add eax, 200000h

	mov eax, 170000h
	mov cr3, eax

	; включение Long mode
	mov	ecx, 0C0000080h
	rdmsr
	or	eax, 1 shl 8
	wrmsr

	; включение табличной трансляции
	mov	eax, cr0
	or	eax, 1 shl 31
	mov	cr0, eax

jmp far fword[LM_addr]
LM_addr:
	phys_addr	dd ?
				dw selektor_LM_CODE

Long_mode:
	use64

	mov rax, 8000h
	jmp rax

;+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
;                   ОПРЕДЕЛЕНИЕ GDT
;+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

align 8
GDT:
descr_NULL 			db 8 dup(0)
descr_LM_CODE    	db 0FFh, 0FFh, 00h, 00h, 00h, 9Ah, 0AFh, 00h
descr_LM_DATA   	db 0FFh, 0FFh, 00h ,00h, 00h, 92h, 0CFh, 00h
descr_PM_CODE 		db 0FFh, 0FFh, 00h, 00h, 00h, 9Ah, 0CFh, 00h
descr_PM_DATA 		db 0FFh, 0FFh, 00h, 00h, 00h, 92h, 0CFh, 00h

GDT_size equ $-GDT-1

label GDTR fword
dw GDT_size
dd ?
