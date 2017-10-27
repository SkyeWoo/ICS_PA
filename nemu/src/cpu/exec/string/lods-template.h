#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

#define instr lods

make_helper(concat(lods_m_, SUFFIX)) {
	current_sreg = R_DS;
	REG(R_EAX) = MEM_R(cpu.esi);
	//R_EAX = swaddr(cpu.esi, DATA_BYTE, R_DS);

	int incdec = (cpu.df)? -DATA_BYTE : DATA_BYTE;

	cpu.esi += incdec;

	print_asm("lods" str(SUFFIX) " %%ds:(%%esi),%%%s", REG_NAME(0));

	return 1;
}

#include "cpu/exec/template-end.h"
