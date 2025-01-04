[org 0x7c00]
[bits 16]

%define ENDL 0x0D, 0x0A



entry:
    jmp start
    nop

bdb_oem:                    db 'MTOO4043'           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 16
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 512
bdb_total_sectors:          dw 0
bdb_media_descriptor_type:  db 248
bdb_sectors_per_fat:        dw 252
bdb_sectors_per_track:      dw 63
bdb_heads:                  dw 16
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0xfb800

ebr_drive_number:           db 0x80
                            db 0
ebr_signature:              db 0x29
ebr_volume_id:              db 0x23, 0x6d, 0xaa, 0xe9
ebr_volume_label:           db 'NO NAME    '
ebr_system_id:              db 'FAT16   '

;
; Prints a string to the screen
; Params:
;   - ds:si points to string
;
puts:
    ; save registers we will modify
    push si
    push ax
    push bx

.loop:
    lodsb               ; loads next character in al
    or al, al           ; verify if next character is null?
    jz .done

    mov ah, 0x0E        ; call bios interrupt
    mov bh, 0           ; set page number to 0
    int 0x10

    jmp .loop

.done:
    pop bx
    pop ax
    pop si    
    ret


start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    push es
    push word .after
    retf

.after:
    push si
    mov si, STAGE1_LOADED
    call puts
    pop si

; si currently hold the address of the partition table

    mov ax, word [bdb_sectors_per_fat]
    mul word [bdb_fat_count]
    add ax, word [bdb_reserved_sectors]         ;505 or 0x3f200

; ax now holds the offset of the fat table
; need to add lba start address for current partition

    add ax, [si + 8]
    mov [root_dir_start_block], ax

    mov [lba], ax
    mov [buffer], word fat_table
    mov [blocks], word 32               ; 512 * 16 * 2 bytes. Or 32 sectors

    mov si, dap
    mov ah, 0x42
    mov dl, 0x80
    int 0x13
    jc noload

;we have root directory at fat_table location

    xor bx, bx
    mov di, fat_table
.search_kernel:
    mov si, STAGE2
    mov cx, 11
    push di
    repe cmpsb
    pop di
    je .kernel_found

    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_kernel

    jmp kernel_not_found_error

.kernel_found:
    mov ax, word [di + 26]
    mul byte [bdb_sectors_per_cluster]
;    shl ax, 4
    add ax, [root_dir_start_block]


    mov [lba], ax
    mov [buffer], word fat_table
    mov [blocks], word 16       ; load the cluster into memoryls 

    mov si, dap
    mov ah, 0x42
    mov dl, 0x80
    int 0x13

    mov si, STAGE2_FOUND
    call puts

    jmp 0:fat_table

noload:
    mov si, NO_LOAD
    call puts
    cli
    jmp halt

kernel_not_found_error:
    mov si, STAGE2_NOT_FOUND
    call puts
    cli
halt:
    jmp halt


STAGE2          db  'STAGE2  BIN'

root_dir_start_block:      dw   0x0
NO_LOAD:                    db  'Not Loaded', ENDL, 0
STAGE1_LOADED:              db  'stage1.bin loaded', ENDL, 0
STAGE2_FOUND:               db  'stage2.bin loading', ENDL, 0
STAGE2_NOT_FOUND:          db   'stage2.bin not found', ENDL, 0

ALIGN(4)

dap:
packet_size     db  0x0
                db  0x0
blocks          dw  0x0
buffer          dd  0x0
lba             dd  0x0


times 510-($-$$) db 0
dw 0xaa55

fat_table:
