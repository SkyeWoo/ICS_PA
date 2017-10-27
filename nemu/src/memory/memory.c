#include "nemu.h"
#include "common.h"
#include <stdio.h>

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);
uint32_t cache_read(hwaddr_t addr);
uint32_t l2_cache_read(hwaddr_t addr);
void cache_write(hwaddr_t addr, size_t len, uint32_t data);
void l2_cache_write(hwaddr_t addr, size_t len, uint32_t data);
lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg);
hwaddr_t page_translate(hwaddr_t addr);

Cache slot[CACHE_SIZE / BLOCK_SIZE];
Cache_L2 slot_l2[CACHE_L2_SIZE / BLOCK_SIZE];
CPU_state cpu;
/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
//	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	uint32_t offset = addr & 0x3f;
	uint8_t temp[4];
	// the largest size of instruction
	memset(temp, 0, sizeof(temp));

	uint32_t i = cache_read(addr);

	if (offset + len >= BLOCK_SIZE) {
		uint32_t j = cache_read(addr + BLOCK_SIZE - offset);
		memcpy(temp, slot[i].data + offset, BLOCK_SIZE - offset);
		memcpy(temp + BLOCK_SIZE - offset, slot[j].data, len - BLOCK_SIZE + offset);
	}
	else memcpy(temp, slot[i].data + offset, len);  

	uint32_t tmp = (uint32_t)temp;
	return unalign_rw(tmp, 4) & (~0u >> ((4 - len) << 3));
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
//	dram_write(addr, len, data);
	cache_write(addr, len, data);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
//	return hwaddr_read(addr, len);
	
	assert(len == 1 || len == 2 || len == 4);

	size_t max_len = ((~addr) & 0xfff) + 1;

	/* data cross the page boundary */
	if (len > max_len) assert(0);
	else {
		hwaddr_t hwaddr;
		if (cpu.cr0.pe && cpu.cr0.pg)
			//hwaddr = page_translate(addr);
			hwaddr = tlb_translate(addr);
		else hwaddr = addr;
		return hwaddr_read(hwaddr, len);
	}
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
//	hwaddr_write(addr, len, data);

	assert(len == 1 || len == 2 || len == 4);

	size_t max_len = ((~addr) & 0xfff) + 1;

	if (len > max_len) assert(0);
	else {
		hwaddr_t hwaddr;
		if (cpu.cr0.pe && cpu.cr0.pg)
			//hwaddr = page_translate(addr);
			hwaddr = tlb_translate(addr);
		else hwaddr = addr;
		hwaddr_write(hwaddr, len, data);
	}
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

