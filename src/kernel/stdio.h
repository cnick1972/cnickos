#pragma once
#include <stdint.h>

void clrscr();
void putc(char c);
void puts(const char* str);
void tprintf(const char* fmt, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t count);
int printf(const char *fmt, ...);