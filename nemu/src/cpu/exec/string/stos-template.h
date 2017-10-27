#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

#define instr stos

make_helper(concat(stos_m_, SUFFIX)) {
	current_sreg = R_ES;
	MEM_W(cpu.edi, REG(R_EAX));
	//swaddr_w(cpu.edi, DATA_BYTE, REG(R_EAX), R_ES);

	int incdec = (cpu.df)? -DATA_BYTE : DATA_BYTE;

	cpu.edi += incdec;

	print_asm("stos" str(SUFFIX) " %s,%%es:(%%edi)", REG_NAME(0));

	return 1;
}

#include "cpu/exec/template-end.h"
