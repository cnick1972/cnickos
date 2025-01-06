#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <cpuid.h>
#include <arch/x86/io.h>
#include <arch/x86/irq.h>
#include <hal/hal.h>

#include "memory.h"
#include "../libs/bootparams.h"


extern uint8_t __bss_start;
extern uint8_t __end;

typedef struct {
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
} model;

static int get_model(model* pModel)
{
    uint32_t unused;
    __cpuid(0, unused, pModel->ebx, pModel->ecx, pModel->edx);
    return 0;
}

void timer(Registers* regs)
{
    //printf(".");
}

void memdump(void* p_ptr, size_t size) {
    void* ptr = p_ptr;
    for(int i = 0; i < size; i += 16) {
        printf("0x%x %x %x %x %x %x %x %x %x  %x %x %x %x %x %x %x %x  |%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c|\n", ptr, *(uint8_t*)ptr, *(uint8_t*)(ptr + 1), *(uint8_t*)(ptr + 2), *(uint8_t*)(ptr + 3),
                                                  *(uint8_t*)(ptr + 4), *(uint8_t*)(ptr + 5), *(uint8_t*)(ptr + 6), *(uint8_t*)(ptr + 7),
                                                  *(uint8_t*)(ptr + 8), *(uint8_t*)(ptr + 9), *(uint8_t*)(ptr + 10), *(uint8_t*)(ptr + 11),
                                                  *(uint8_t*)(ptr + 12), *(uint8_t*)(ptr + 13), *(uint8_t*)(ptr + 14), *(uint8_t*)(ptr + 15),
                                                  *(char*)ptr, *(char*)(ptr + 1), *(char*)(ptr + 2), *(char*)(ptr + 3),
                                                  *(char*)(ptr + 4), *(char*)(ptr + 5), *(char*)(ptr + 6), *(char*)(ptr + 7),
                                                  *(char*)(ptr + 8), *(char*)(ptr + 9), *(char*)(ptr + 10), *(char*)(ptr + 11),
                                                  *(char*)(ptr + 12), *(char*)(ptr + 13), *(char*)(ptr + 14), *(char*)(ptr + 15));
        ptr += 16;
    }



}

char* buffer = (char*)0xb8000;

void __attribute__((section(".entry"))) start(BootParams* params)
{
    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();

    HAL_Initialize();
    x86_IRQ_RegisterHandler(0, timer);

    char buffer[13] = "Unknown\0";
    
    if(x86_cpuid()) {
        model myModel;
        get_model(&myModel);

        memset(&buffer, 0, 13);
        memcpy(buffer, &myModel, 12);
    }

    printf("Welcome to the kernel\n");
    printf("CPU Model: %s\n", buffer);
    printf("Boot drive: 0x%x\n", params->BootDevice);

    printf("Memory regions count: %d\n", params->Memory.RegionCount);
    for(int i = 0; i < params->Memory.RegionCount; i++) {
        printf("MEM: region=%x\tstart=0x%llx\tlength=0x%llx\ttype=%x\n", i,
                                                                      params->Memory.Regions[i].Begin,
                                                                      params->Memory.Regions[i].Length,
                                                                      params->Memory.Regions[i].Type);


    }

    char point[8] = "RSD PTR ";
    char* mem = (char*)0xe0000;

    for(uint32_t t = 0; t < 0x1ffff; t += 8)
    {
        if(memcmp(point, mem + t, 8)) {

        }
        else
        {
            printf("Match Found at 0x%x\n", mem + t);
        }
    }
    memdump((void*)0x600, 32);

end:
    for(;;);
}