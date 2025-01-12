[bits 32]

global LoadPageDirectory
LoadPageDirectory:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov eax, [bp + 8]
    mov cr3, eax
    
    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret


global enablePaging
enablePaging:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax


    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret