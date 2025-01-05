#include "hal.h"
#include <arch/x86/gdt.h>
#include <arch/x86/idt.h>
#include <arch/x86/isr.h>
#include <arch/x86/irq.h>

void HAL_Initialize()
{
    x86_GDT_Initialize();
    x86_IDT_Initialize();
    x86_ISR_Initialize();
    x86_IRQ_Initialize();
}