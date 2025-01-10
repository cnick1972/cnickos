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
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed))RSDP_t;

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
        printf("MEM: region=%d start=0x%08x length=0x%08x type=%d\n", i,
                                                                      (uint32_t)params->Memory.Regions[i].Begin,
                                                                      (uint32_t)params->Memory.Regions[i].Length,
                                                                      params->Memory.Regions[i].Type);


    }

    char point[8] = "RSD PTR ";
    char* mem = (char*)0xe0000;

    RSDP_t* pRSDP;

    for(uint32_t t = 0; t < 0x1ffff; t += 8)
    {
        if(memcmp(point, mem + t, 8)) {

        }
        else
        {
            printf("Match Found at 0x%08x\n", mem + t);
            pRSDP = (RSDP_t*)(mem + t);
        }
    }

    printf("Signature %s, RSDT Address 0x%08x\n", pRSDP->Signature, pRSDP->RsdtAddress);

end:
    for(;;);
}