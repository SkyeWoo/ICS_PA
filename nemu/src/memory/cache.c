#include "nemu.h"
#include "common.h"
#include <stdlib.h>
#include "burst.h"

Cache slot[CACHE_SIZE / BLOCK_SIZE];
Cache_L2 slot_l2[CACHE_L2_SIZE / BLOCK_SIZE];
#define EIGHT_WAY 8
// c = 10, q = 7, s = 3, b = 6;
#define SIXTEEN_WAY 16
// c = 16, q = 12, s = 4, b = 6;

uint32_t dram_read(hwaddr_t addr, size_t len);
void dram_write(hwaddr_t addr, size_t len, uint32_t data);
void ddr3_read(hwaddr_t, void*);
void ddr3_write(hwaddr_t, void*, uint8_t*);

void init_cache() {
	int i = 0;
	for (; i < CACHE_SIZE / BLOCK_SIZE; i++) {
		slot[i].valid = false;
		slot[i].tag = 0;
		memset(slot[i].data, 0, BLOCK_SIZE);
	}

	for (i = 0; i < CACHE_L2_SIZE / BLOCK_SIZE; i++) {
		slot_l2[i].valid = false;
		slot_l2[i].dirty = false;
		slot_l2[i].tag = 0;
		memset(slot_l2[i].data, 0, BLOCK_SIZE);
	}
}

uint32_t l2_cache_read(hwaddr_t addr);
void l2_cache_write(hwaddr_t addr, size_t len, uint32_t data);

uint32_t cache_read(hwaddr_t addr) {
	uint32_t group = (addr >> 6) & 0x7f;
	
	int i;
	bool hit = false;
	for (i = group * EIGHT_WAY; i < (group + 1) * EIGHT_WAY; i++) {
		if ((slot[i].tag == (addr >> 13)) && slot[i].valid) {
			hit = true; break;
		}
	}

	/* no hit, write into cache */
	if (!hit) {
		/* look for a valid slot */
		for (i = group * EIGHT_WAY; i < (group + 1) * EIGHT_WAY; i++)
			if (!slot[i].valid) break;

		/* no valid slot, write in ramdom */
		if (i == (group + 1) * EIGHT_WAY) {
			srand(0);
			i = group * EIGHT_WAY + rand() % EIGHT_WAY;
		}

		slot[i].valid = true;
		slot[i].tag = addr >> 13;

		uint32_t j = l2_cache_read(addr);
		memcpy(slot[i].data, slot_l2[j].data, BLOCK_SIZE);
	}

	return i;
}

uint32_t l2_cache_read(hwaddr_t addr) {
	uint32_t group = (addr >> 6) & 0xfff;
	uint32_t offset = (addr >> 6) << 6;
	// block start at offset
	
	int i;
	bool hit = false;
	for (i = group * SIXTEEN_WAY; i < (group + 1) * SIXTEEN_WAY; i++)
		if ((slot_l2[i].tag == (addr >> 18)) && slot_l2[i].valid) {
			hit = true; break;
		}

	/* no hit, write into cache_l2 */
	if (!hit) {
		/* look for a valid slot */
		for (i = group * SIXTEEN_WAY; i < (group + 1) * SIXTEEN_WAY; i++)
			if (!slot_l2[i].valid) break;

		int j;
		/* no valid slot, write in ramdom */
		if (i == (group + 1) * SIXTEEN_WAY) {
			srand(0);
			i = group * SIXTEEN_WAY + rand() % SIXTEEN_WAY;

			/* has been modified, write back to dram */
			if (slot_l2[i].dirty) {
				//uint8_t mask[BURST_LEN * 2];
				//memset(mask, 1, BURST_LEN * 2);
				for (j = 0; j < BLOCK_SIZE; j++) dram_write(offset + j, 1, slot_l2[i].data[j]);
				slot_l2[i].dirty = false;
				//for (j = 0; j < BLOCK_SIZE / BURST_LEN; j++) ddr3_write(offset + j * BURST_LEN, slot_l2[i].data + j * BURST_LEN, mask);
			}
		}

		slot_l2[i].valid = true;
		slot_l2[i].tag = addr >> 18;
		slot_l2[i].dirty = false;
		for (j = 0; j < BLOCK_SIZE; j++) slot_l2[i].data[j] = dram_read(offset + j, 1);
	}

	return i;
}


void cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t group = (addr >> 6) & 0x7f;
	uint32_t offset = addr & 0x3f;

	int i;
	bool hit = false;
	for (i = group * EIGHT_WAY; i < (group + 1) * EIGHT_WAY; i++)
		if ((slot[i].tag == (addr >> 13)) && slot[i].valid) {
			hit = true; break;
		}

	uint32_t temp = data;
	if (hit) {
		int j;
		for (j = 0; j < len; j++) {
			slot[i].data[offset + j] = temp & 0xff;
			temp >>= 8;
		}
	}
	
	dram_write(addr, len, data);
	l2_cache_write(addr, len, data);
}

void l2_cache_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t group = (addr >> 6) & 0xfff;
	uint32_t offset = (addr >> 6) << 6;
	// block start at offset
	
	int i;
	bool hit = false;
	for (i = group * SIXTEEN_WAY; i < (group + 1) * SIXTEEN_WAY; i++)
		if ((slot_l2[i].tag == (addr >> 13)) && slot_l2[i].valid) {
			hit = true; break;
		}

	if (!hit) i = l2_cache_read(addr);

	int j;
	for (j = 0; j < BLOCK_SIZE; j++) slot_l2[i].data[j] = dram_read(offset + j, 1);
	slot_l2[i].dirty = true;
}
