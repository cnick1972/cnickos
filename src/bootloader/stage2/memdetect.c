#include <stdint.h>
#include "memdetect.h"
#include "x86.h"
#include "stdio.h"

#define MAX_REGIONS 256

MemoryRegion g_MemRegions[MAX_REGIONS];
int g_MemRegionCount;

void MemoryDetect(MemoryInfo* memoryInfo)
{
    E820MemoryBlock block;

    uint32_t continuation = 0;
    int ret;
    g_MemRegionCount = 0;

    ret = x86_E820GetNextBlock(&block, &continuation);

    while(ret > 0 && continuation != 0)
    {
        g_MemRegions[g_MemRegionCount].Begin = block.Base;
        g_MemRegions[g_MemRegionCount].Length = block.Length;
        g_MemRegions[g_MemRegionCount].Type = block.Type;
        g_MemRegions[g_MemRegionCount].ACPI = block.ACPI;
        ++g_MemRegionCount;
        ret = x86_E820GetNextBlock(&block, &continuation);
    }

    uint16_t MemoryHi, MemoryLo;

    x86_GetBiosMemory64(&MemoryLo, &MemoryHi);

    memoryInfo->RegionCount = g_MemRegionCount;
    memoryInfo->Regions = g_MemRegions;
    memoryInfo->memoryLO = MemoryLo;
    memoryInfo->memoryHI = MemoryHi;
}