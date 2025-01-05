#pragma once

#include <stdint.h>

void x86_PIC_Configure(uint8_t offsetPIC1, uint8_t offsetPIC2);
void x86_PIC_SendEndOfInterrupt(int irq);
void x86_PIC_Disable();
void x86_PIC_Mask(int irq);
void x86_PIC_Unmask(int irq);
uint16_t x86_PIC_ReadIrqRequestRegister();
uint16_t x86_PIC_ReadInServiceRegister();