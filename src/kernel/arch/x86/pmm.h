#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef	uint32_t physical_addr;

void pmm_init(size_t memSize, physical_addr* bitmap);
void pmm_init_region(physical_addr base, size_t size);
void pmm_deinit_region(physical_addr base, size_t size);
void* pmm_alloc_block();
void pmm_free_block(void* p);
void pmm_free_blocks(void* p, size_t size);
void* pmm_alloc_blocks(size_t size);
size_t	pmm_get_memory_size();
uint32_t pmm_get_block_count();
uint32_t pmm_get_use_block_count();
uint32_t pmm_get_free_block_count();
uint32_t pmm_get_block_size();