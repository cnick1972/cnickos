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

        printf("E820: base=0x%llx length=0x%llx type=0x%x\n", block.Base, block.Length, block.Type);

        ret = x86_E820GetNextBlock(&block, &continuation);
    }
    memoryInfo->RegionCount = g_MemRegionCount;
    memoryInfo->Regions = g_MemRegions;
}