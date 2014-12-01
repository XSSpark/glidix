section .bootstrap32
bits 32

; multiboot header
dd 0x1BADB002
dd 65539
dd -(0x1BADB002 + 65539)
dd 0x10000			; base addr
dd 0x10000			; phys base addr
dd 0
dd 0
dd _start

[extern kmain]
[global _start]
_start:
	cli

	; clear the screen first.
	mov edi,	0xB8000
	mov ax,		0x0700
	mov ecx,	80*25
	rep stosw

	pushfd               ; Store the FLAGS-register.
	pop eax              ; Restore the A-register.
	mov ecx, eax         ; Set the C-register to the A-register.
	xor eax, 1 << 21     ; Flip the ID-bit, which is bit 21.
	push eax             ; Store the A-register.
	popfd                ; Restore the FLAGS-register.
	pushfd               ; Store the FLAGS-register.
	pop eax              ; Restore the A-register.
	push ecx             ; Store the C-register.
	popfd                ; Restore the FLAGS-register.
	xor eax, ecx         ; Do a XOR-operation on the A-register and the C-register.
	jz  _crash           ; The zero flag is set, no CPUID.

	mov eax, 0x80000000    ; Set the A-register to 0x80000000.
	cpuid                  ; CPU identification.
	cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
	jb _crash              ; It is less, there is no long mode.

	mov eax, 0x80000001    ; Set the A-register to 0x80000001.
	cpuid                  ; CPU identification.
	test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
	jz _crash              ; They aren't, there is no long mode.

	; clear page tables.
	mov edi, 0x1000    ; Set the destination index to 0x1000.
	mov cr3, edi       ; Set control register 3 to the destination index.
	xor eax, eax       ; Nullify the A-register.
	mov ecx, 4096      ; Set the C-register to 4096.
	rep stosd          ; Clear the memory.
	mov edi, cr3       ; Set the destination index to control register 3.

	mov dword [edi], 0x2003      ; Set the double word at the destination index to 0x2003.
	add edi, 0x1000              ; Add 0x1000 to the destination index.
	mov dword [edi], 0x3003      ; Set the double word at the destination index to 0x3003.
	add edi, 0x1000              ; Add 0x1000 to the destination index.
	mov dword [edi], 0x4003      ; Set the double word at the destination index to 0x4003.
	add edi, 0x1000              ; Add 0x1000 to the destination index.

	mov ebx, 0x00000003          ; Set the B-register to 0x00000003.
	mov ecx, 512                 ; Set the C-register to 512.
 
	.SetEntry:
	mov DWORD [edi], ebx         ; Set the double word at the destination index to the B-register.
	add ebx, 0x1000              ; Add 0x1000 to the B-register.
	add edi, 8                   ; Add eight to the destination index.
	loop .SetEntry               ; Set the next entry.

	; enable PAE paging
	mov eax, cr4                 ; Set the A-register to control register 4.
	or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
	mov cr4, eax                 ; Set control register 4 to the A-register.

	; enable long mode
	mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
	rdmsr                        ; Read from the model-specific register.
	or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
	wrmsr                        ; Write to the model-specific register.

	; enable paging.
	mov eax, cr0                 ; Set the A-register to control register 0.
	or eax, 1 << 31              ; Set the PG-bit, which is the 32nd bit (bit 31).
	mov cr0, eax                 ; Set control register 0 to the A-register.

	; go to 64-bit long mode.
	lgdt [GDT64.Pointer]
	jmp GDT64.Code:_bootstrap64

	hlt

_crash:
	cli

	mov edi, 0xB8000
	mov al, '!'
	mov ah, 0x03
	mov ecx, 80*26
	rep stosw

	hlt

GDT64:                           ; Global Descriptor Table (64-bit).
	.Null: equ $ - GDT64         ; The null descriptor.
	dw 0                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 0                         ; Access.
	db 0                         ; Granularity.
	db 0                         ; Base (high).
	.Code: equ $ - GDT64         ; The code descriptor.
	dw 0                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 10011000b                 ; Access.
	db 00100000b                 ; Granularity.
	db 0                         ; Base (high).
	.Data: equ $ - GDT64         ; The data descriptor.
	dw 0                         ; Limit (low).
	dw 0                         ; Base (low).
	db 0                         ; Base (middle)
	db 10010000b                 ; Access.
	db 00000000b                 ; Granularity.
	db 0                         ; Base (high).
	.Pointer:                    ; The GDT-pointer.
	dw $ - GDT64 - 1             ; Limit.
	dq GDT64                     ; Base.

bits 64
_bootstrap64:
	; load 64-bit data segments.
	mov ax,		GDT64.Data
	mov ds,		ax
	mov es,		ax
	mov fs,		ax
	mov gs,		ax

	; set up a stack.
	mov rsp,	stack
	add rsp,	0x4000

	; go to the C part.
	call		kmain

	; if the C kernel returns, halt the CPU.
	cli
	hlt

stack resq 0x4000
