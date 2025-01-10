#include "pmm.h"
#include "../../memory.h"

//! 8 blocks per byte
#define PMMNGR_BLOCKS_PER_BYTE 8

//! block size (4k)
#define PMMNGR_BLOCK_SIZE	4096

//! block alignment
#define PMMNGR_BLOCK_ALIGN	PMMNGR_BLOCK_SIZE


//! size of physical memory
static	uint32_t	_mmngr_memory_size = 0;

//! number of blocks currently in use
static	uint32_t	_mmngr_used_blocks = 0;

//! maximum number of available memory blocks
static	uint32_t	_mmngr_max_blocks = 0;

//! memory map bit array. Each bit represents a memory block
static	uint32_t*	_mmngr_memory_map = 0;

//! set any bit (frame) within the memory map bit array
void mmap_set (int bit) {

  _mmngr_memory_map[bit / 32] |= (1 << (bit % 32));
}

//! unset any bit (frame) within the memory map bit array
void mmap_unset (int bit) {

  _mmngr_memory_map[bit / 32] &= ~ (1 << (bit % 32));
}

bool mmap_test (int bit) {

	return _mmngr_memory_map[bit / 32] &  (1 << (bit % 32));
}


int mmap_first_free () {

	//! find the first free bit
	for (uint32_t i=0; i< pmm_get_block_count() /32; i++)
		if (_mmngr_memory_map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {				//! test each bit in the dword

				int bit = 1 << j;
				if (! (_mmngr_memory_map[i] & bit) )
					return i*4*8+j;
			}

	return -1;
}

int mmap_first_free_s (size_t size) {

	if (size==0)
		return -1;

	if (size==1)
		return mmap_first_free ();

	for (uint32_t i=0; i<pmm_get_block_count() /32; i++)
		if (_mmngr_memory_map[i] != 0xffffffff)
			for (int j=0; j<32; j++) {	//! test each bit in the dword

				int bit = 1<<j;
				if (! (_mmngr_memory_map[i] & bit) ) {

					int startingBit = i*32;
					startingBit+=bit;		//get the free bit in the dword at index i

					uint32_t free=0; //loop through each bit to see if its enough space
					for (uint32_t count=0; count<=size;count++) {

						if (! mmap_test (startingBit+count) )
							free++;	// this bit is clear (free frame)

						if (free==size)
							return i*4*8+j; //free count==size needed; return index
					}
				}
			}

	return -1;
}

void pmm_init(size_t memSize, physical_addr* bitmap)
{
    _mmngr_memory_size	=	memSize;
    _mmngr_memory_map	=	(uint32_t*) bitmap;
    _mmngr_max_blocks	=	(pmm_get_memory_size() * 1024) / PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks	=	_mmngr_max_blocks;

    memset(_mmngr_memory_map, 0xff, pmm_get_block_count() / PMMNGR_BLOCKS_PER_BYTE);
}

void pmm_init_region(physical_addr base, size_t size)
{
	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;

	for (; blocks>=0; blocks--) {
		mmap_unset (align++);
		_mmngr_used_blocks--;
	}

	mmap_set (0);	//first block is always set. This insures allocs cant be 0
}

void pmm_deinit_region(physical_addr base, size_t size) 
{

	int align = base / PMMNGR_BLOCK_SIZE;
	int blocks = size / PMMNGR_BLOCK_SIZE;

	for (; blocks>=0; blocks--) {
		mmap_set (align++);
		_mmngr_used_blocks++;
	}

}

void* pmm_alloc_block() {

	if (pmm_get_free_block_count() <= 0)
		return 0;	//out of memory

	int frame = mmap_first_free ();

	if (frame == -1)
		return 0;	//out of memory

	mmap_set (frame);

	physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks++;

	return (void*)addr;
}

void pmm_free_block(void* p) {

	physical_addr addr = (physical_addr)p;
	int frame = addr / PMMNGR_BLOCK_SIZE;

	mmap_unset (frame);

	_mmngr_used_blocks--;
}

void pmm_free_blocks(void* p, size_t size) {

	physical_addr addr = (physical_addr)p;
	int frame = addr / PMMNGR_BLOCK_SIZE;

	for (uint32_t i=0; i<size; i++)
		mmap_unset (frame+i);

	_mmngr_used_blocks-=size;
}

void* pmm_alloc_blocks(size_t size) {

	if (pmm_get_free_block_count() <= size)
		return 0;	//not enough space

	int frame = mmap_first_free_s (size);

	if (frame == -1)
		return 0;	//not enough space

	for (uint32_t i=0; i<size; i++)
		mmap_set (frame+i);

	physical_addr addr = frame * PMMNGR_BLOCK_SIZE;
	_mmngr_used_blocks+=size;

	return (void*)addr;
}

size_t	pmm_get_memory_size() {

	return _mmngr_memory_size;
}

uint32_t pmm_get_block_count() {

	return _mmngr_max_blocks;
}

uint32_t pmm_get_use_block_count() {

	return _mmngr_used_blocks;
}

uint32_t pmm_get_free_block_count() {

	return _mmngr_max_blocks - _mmngr_used_blocks;
}

uint32_t pmm_get_block_size() {

	return PMMNGR_BLOCK_SIZE;
}