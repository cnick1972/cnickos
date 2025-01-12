[bits 32]

;
; uint32_t* ASMCALL GetCurrentPageDirectory();
;
global GetCurrentPageDirectory
GetCurrentPageDirectory:
    mov eax, cr3
    ret

;
; void ASMCALL SetPageDirectory(uint32_t* PageDir);
;
global SetPageDirectory
SetPageDirectory:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    mov eax, [bp + 8]
    mov cr3, eax
    
    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret