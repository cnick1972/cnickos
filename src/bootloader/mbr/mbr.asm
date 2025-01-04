[bits 16]
[org 0x0600]

    cli
    xor ax,ax
    mov ss,ax
    mov sp,0x7c00
    mov si,sp
    push ax
    pop es
    push ax
    pop ds
    sti
    cld

    mov di,0x600
    mov cx,0x100
    repnz movsw


    jmp 0x0:LowStart

LowStart:
    mov si, tab
    mov bl, 4

next:
    cmp byte [si], 0x80
    je boot
    cmp byte [si], 0x00
    jne bad
    add si, 16
    dec bl
    jnz next
    int 0x18

boot:
;    mov dx, [si]
;    mov cx, [si + 2]
    mov eax, [si + 8]
    mov [dap.lba], eax
    mov [dap.bufferOffset], word 0x7c00
    mov [dap.blocks], word 0x1
    mov [dap.packet_size], word 0x10

    mov bp, si

next1:
    add si, 16
    dec bl
    jz tabok
    cmp byte [si], 0
    je next1
bad:
    mov si, m1

msg:
    lodsb
    cmp al, 0
    je hold
    push si
    mov bx, 7
    mov ah, 14
    int 0x10
    pop si
    jmp msg

hold:   jmp hold

tabok:
    mov di, 5

rdboot:

   ;mov bx, 0x7c00
   ;mov ax, 0x0201

   mov ah, 0x42
   mov si, dap

    push di
    int 0x13
    pop di
    jnc goboot
    xor ax, ax
    int 0x13
    dec di
    jnz rdboot
    mov si, m2
    jmp msg

goboot:
    mov si, m3
    mov di, 0x7dfe
    cmp word [di], 0xaa55
    jne msg
    mov si, bp
    jmp 0x0:0x7c00

        

m1      db      'Invalid partition table', 0x00
m2      db      'Error loading operating system', 0x00
m3      db      'Missing operating system', 0x00

dap:
    .packet_size    db  0x0
                    db  0x0
    .blocks         dw  0x0
    .bufferOffset   dw  0x0
    .bufferSegemnt  dw  0x0
    .lba            dd  0x0

times 0x1be - ($-$$) db 0

tab:

