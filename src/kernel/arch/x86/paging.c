#include "paging.h"
#include "../../memory.h"

void relocatePageDirectory(uint32_t* newLocation)
{
    memcpy(newLocation, GetCurrentPageDirectory(), 4096);
    SetPageDirectory(newLocation);
}