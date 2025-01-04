#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t id;
} DISK;

bool DISK_Initialize(DISK* disk, uint8_t driveNumber);
bool DISK_ReadSectors(DISK* disk, uint32_t lba, uint8_t sectors, void* lowerDataOut);