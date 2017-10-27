#include "common.h"
#include "memory.h"
#include "cpu/reg.h"

uint32_t hwaddr_read(hwaddr_t, size_t);

hwaddr_t page_translate(lnaddr_t lnaddr) {
	uint16_t dir = (lnaddr >> 22) & 0x3ff;
	hwaddr_t pde_addr = (cpu.cr3.pdbr << 12) + (dir << 2);
	uint32_t pde = hwaddr_read(pde_addr, 4);
	Assert(pde & 1, "PDE not present!");

	uint16_t page = (lnaddr >> 12) & 0x3ff;
	hwaddr_t pte_addr = (pde & 0xfffff000) + (page << 2);
	uint32_t pte = hwaddr_read(pte_addr, 4);
	Assert(pte & 1, "PTE not present!");

	uint16_t offset = lnaddr & 0xfff;
	return ((pte & 0xfffff000) | offset);
}
