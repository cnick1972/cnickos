#pragma once

#include <stdint.h>

#define ASMCALL __attribute__((cdecl))


uint32_t* ASMCALL GetCurrentPageDirectory();
void ASMCALL SetPageDirectory(uint32_t* PageDir);
void relocatePageDirectory(uint32_t* newLocation);