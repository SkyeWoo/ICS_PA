#include "nemu.h"
#include <stdlib.h>
#include <time.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
	srand(time(0));
	uint32_t sample[8];
	uint32_t eip_sample = rand();
	cpu.eip = eip_sample;

	int i;
	for(i = R_EAX; i <= R_EDI; i ++) {
		sample[i] = rand();
		reg_l(i) = sample[i];
		assert(reg_w(i) == (sample[i] & 0xffff));
	}

	assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
	assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
	assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
	assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
	assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
	assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
	assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
	assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

	assert(sample[R_EAX] == cpu.eax);
	assert(sample[R_ECX] == cpu.ecx);
	assert(sample[R_EDX] == cpu.edx);
	assert(sample[R_EBX] == cpu.ebx);
	assert(sample[R_ESP] == cpu.esp);
	assert(sample[R_EBP] == cpu.ebp);
	assert(sample[R_ESI] == cpu.esi);
	assert(sample[R_EDI] == cpu.edi);

	assert(eip_sample == cpu.eip);
}

void sreg_load(uint8_t sreg) {
	Assert(cpu.cr0.pe, "Not in protection mode");
	uint16_t sel = cpu.sr[sreg].selector;
	uint16_t index = sel >> 3;
	uint8_t ti = (sel >> 2) & 1;

	Assert(ti == 0, "LDT is not supported");
	Assert(index * 8 < cpu.gdtr.limit, "segment selector out of limit");

	uint64_t SegDesc = ((uint64_t) lnaddr_read(cpu.gdtr.base + index * 8 + 4, 4) << 32) | lnaddr_read(cpu.gdtr.base + index * 8, 4);

	/* presen */
	uint8_t p = (SegDesc >> 47);
	Assert(p, "Segment not presen");

	uint32_t limit = (SegDesc & 0xffff) | ((SegDesc >> 32) & 0xf0000);
	cpu.sr[sreg].base = ((SegDesc >> 16) & 0xffffff) | ((SegDesc >> 32) & 0xff000000);

	/* granularity */
	uint8_t g = (SegDesc >> 55);
	/* if (g == 1) 4kB else bit */
	if (g) limit <<= 12;

	cpu.sr[sreg].limit = limit;
}
