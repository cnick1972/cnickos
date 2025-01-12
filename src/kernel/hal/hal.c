#include "hal.h"
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>

void HAL_Initialize()
{
    x86_GDT_Initialize();
    x86_IDT_Initialize();
    x86_ISR_Initialize();
    x86_IRQ_Initialize();
}