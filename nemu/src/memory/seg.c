#include "nemu.h"

lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg) {
	if (cpu.cr0.pe == 0) return addr;

	Assert(sreg < 6, "invalid sreg");

	uint16_t sel = cpu.sr[sreg].selector;
	uint16_t index = (sel >> 3) & 0x1fff;
	uint8_t ti = (sel >> 2) & 1;
	Assert(ti == 0, "ldt not implemented");
	Assert((index << 3) <= cpu.gdtr.limit, "gdt index out of range");

	Assert(addr + len < cpu.sr[sreg].limit, "out of segment");
	
	return cpu.sr[sreg].base + addr;
}
