#pragma once
#include <stdint.h>
#include <stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);


bool __attribute__((cdecl)) x86_Disk_Reset(uint8_t drive);

typedef struct {
    uint8_t     size;
    uint8_t     reserved;
    uint16_t    NumberOfBlocks;
    uint16_t    bufferOffset;
    uint16_t    BufferSegment;
    uint64_t    startingBlock;
} __attribute__((packed)) DAP;


bool __attribute__((cdecl)) x86_Disk_Read_LBA(uint8_t drive,
                                              DAP* dap);

typedef struct {
    uint64_t Base;
    uint64_t Length;
    uint32_t Type;
    uint32_t ACPI;
} E820MemoryBlock;

enum E820MemoryBlockType
{
    E820_USABLE             = 1,
    E820_RESERVED           = 2,
    E820_ACPI_RECLAIMABLE   = 3,
    E820_ACPI_NVS           = 4,
    E820_BAD_MEMORY         = 5,
};

int __attribute__((cdecl)) x86_E820GetNextBlock(E820MemoryBlock* block, uint32_t* continuationId);
