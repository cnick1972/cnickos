#pragma once

#include <stdint.h>

#define ASMCALL __attribute__((cdecl))


void ASMCALL LoadPageDirectory(uint32_t* pageDirectory);
void ASMCALL enablePaging();