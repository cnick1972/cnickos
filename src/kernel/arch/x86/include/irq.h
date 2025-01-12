#pragma once
#include "isr.h"

typedef void (*IRQHandler)(Registers* regs);

void x86_IRQ_Initialize();
void x86_IRQ_RegisterHandler(int irq, IRQHandler handler);