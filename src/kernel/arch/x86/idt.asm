[bits 32]

;void __attribute__((cdecl)) x86_IDT_Load(IDTDescriptor* idtDescriptor)
global x86_IDT_Load
x86_IDT_Load:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp         ; initialize new call frame

    mov eax, [ebp + 8]
    lidt [eax]

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret