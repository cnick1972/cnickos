#include <stdint.h>
#include <pic.h>
#include <io.h>

#define PIC1_COMMAND_PORT           0x20
#define PIC1_DATA_PORT              0x21
#define PIC2_COMMAND_PORT           0xA0
#define PIC2_DATA_PORT              0xA1

enum {
    PIC_ICW1_ICW4               = 0b00000001,       // 0x01
    PIC_ICW1_SINGLE             = 0b00000010,       // 0x02
    PIC_ICW1_INTERVAL4          = 0b00000100,       // 0x04
    PIC_ICW1_LEVEL              = 0b00001000,       // 0x08
    PIC_ICW1_INITIALIZE         = 0b00010000,       // 0x10
} PIC_ICW1;

enum {
    PIC_ICW4_8086               = 0b00000001,       // 0x01
    PIC_ICW4_AUTO_EOI           = 0b00000010,       // 0x02
    PIC_ICW4_BUFFER_MASTER      = 0b00000100,       // 0x04
    PIC_ICW4_BUFFER_SLAVE       = 0b00000000,       // 0x00
    PIC_ICW4_BUFFERED           = 0b00001000,       // 0x08
    PIC_ICW4_SFNM               = 0b00010000,       // 0x10
} PIC_ICW4;

enum {
    PIC_CMD_END_OF_INTERRUPT    = 0x20,
    PIC_CMD_READ_IRR            = 0x0A,
    PIC_CMD_READ_ISR            = 0x0B,
} PIC_CMD;

void x86_PIC_Configure(uint8_t offsetPIC1, uint8_t offsetPIC2)
{
    x86_outb(PIC1_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    x86_iowait();
    x86_outb(PIC2_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    x86_iowait(); 

    x86_outb(PIC1_DATA_PORT, offsetPIC1);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, offsetPIC2);
    x86_iowait();

    x86_outb(PIC1_DATA_PORT, 0x04);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, 0x02);
    x86_iowait();

    x86_outb(PIC1_DATA_PORT, PIC_ICW4_8086);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, PIC_ICW4_8086);
    x86_iowait();

    x86_outb(PIC1_DATA_PORT, 0);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, 0);
    x86_iowait();
}

void x86_PIC_SendEndOfInterrupt(int irq)
{
    if(irq >= 8)
        x86_outb(PIC2_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
    x86_outb(PIC1_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
}

void x86_PIC_Disable()
{
    x86_outb(PIC1_DATA_PORT, 0xFF);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, 0xFF);
    x86_iowait();
}

void x86_PIC_Mask(int irq)
{
    uint8_t port;
    if (irq < 8)
        port = PIC1_DATA_PORT;
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = x86_inb(port);
    x86_outb(port, mask | (1 << irq));
}

void x86_PIC_Unmask(int irq)
{
    uint8_t port;
    if (irq < 8)
        port = PIC1_DATA_PORT;
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = x86_inb(port);
    x86_outb(port, mask & ~(1 << irq));
}

uint16_t x86_PIC_ReadIrqRequestRegister()
{
    x86_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_IRR);
    x86_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_IRR);
    return ((uint16_t)x86_inb(PIC1_COMMAND_PORT)) | (((uint16_t)x86_inb(PIC2_COMMAND_PORT)) << 8);
}

uint16_t x86_PIC_ReadInServiceRegister()
{
    x86_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_ISR);
    x86_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_ISR);
    return ((uint16_t)x86_inb(PIC1_COMMAND_PORT)) | (((uint16_t)x86_inb(PIC2_COMMAND_PORT)) << 8);
}