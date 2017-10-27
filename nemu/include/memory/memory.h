#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#define HW_MEM_SIZE (128 * 1024 * 1024)
#define BLOCK_SIZE 64
#define CACHE_SIZE (64*1024)
#define CACHE_L2_SIZE (4*1024*1024)
#define TLB_SIZE 64

typedef struct {
	bool valid;
	int tag;
	uint8_t data[BLOCK_SIZE];
} Cache;

typedef struct {
	bool valid, dirty;
	int tag;
	uint8_t data[BLOCK_SIZE];
} Cache_L2;

typedef struct {
	uint32_t tag;
	uint32_t paddr;
	bool valid;
} TLB;

extern Cache slot[CACHE_SIZE / BLOCK_SIZE];
extern Cache_L2 slot_l2[CACHE_L2_SIZE / BLOCK_SIZE];
extern TLB tlb[TLB_SIZE];
extern uint8_t *hw_mem;

/* convert the hardware address in the test program to virtual address in NEMU */
#define hwa_to_va(p) ((void *)(hw_mem + (unsigned)p))
/* convert the virtual address in NEMU to hardware address in the test program */
#define va_to_hwa(p) ((hwaddr_t)((void *)p - (void *)hw_mem))

#define hw_rw(addr, type) *(type *)({\
	Assert(addr < HW_MEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
	hwa_to_va(addr); \
})

uint32_t swaddr_read(swaddr_t, size_t, uint8_t);
uint32_t lnaddr_read(lnaddr_t, size_t);
uint32_t hwaddr_read(hwaddr_t, size_t);
void swaddr_write(swaddr_t, size_t, uint32_t, uint8_t);
void lnaddr_write(lnaddr_t, size_t, uint32_t);
void hwaddr_write(hwaddr_t, size_t, uint32_t);
lnaddr_t seg_translate(swaddr_t, size_t, uint8_t);
hwaddr_t page_translate(lnaddr_t);
hwaddr_t tlb_translate(lnaddr_t);
#endif
