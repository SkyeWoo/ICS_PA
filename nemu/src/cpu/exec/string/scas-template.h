#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

#define instr scas

make_helper(concat(scas_m_, SUFFIX)) {
	DATA_TYPE a = REG(0);
	current_sreg = R_ES;
	uint32_t b = MEM_R(cpu.edi);
	//uint32_t b = swaddr_r(cpu.edi, DATA_BYTE, R_ES);
	DATA_TYPE temp = a - b;

	int incdec = (cpu.df)? -DATA_BYTE : DATA_BYTE;

	cpu.edi += incdec;
	
	cpu.of = (MSB(a) != MSB(b)) && (MSB(a) != MSB(temp))? 1 : 0;
	cpu.af = ((a & 0x8) > (b & 0x8))? 0 : 1;
	cpu.cf = ((long long)((long long)a - (long long)b) >> (8 * DATA_BYTE)) & 1;
	cpu.sf = MSB(temp);
	cpu.zf = !temp;
	int count = 0;
	for (; temp; count++) temp &= temp - 1;
	cpu.pf = (count % 2)? 1 : 0;

	print_asm("scas" str(SUFFIX) " %%es:(%%edi),%%%s", REG_NAME(0));

	return 1;
}

#include "cpu/exec/template-end.h"
