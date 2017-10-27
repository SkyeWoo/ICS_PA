#include "cpu/exec/template-start.h"

#define instr movs

make_helper(concat(movs_m2m_, SUFFIX)) {	
	ops_decoded.is_operand_size_16 = (DATA_BYTE == 2)? 1 : 0;
	
	current_sreg = R_DS;
	uint32_t temp = MEM_R(cpu.esi);
	current_sreg = R_ES;
	MEM_W(cpu.edi, temp);
	//swaddr_w(cpu.edi, DATA_BYTE, swaddr_r(cpu.esi, DATA_BYTE, R_DS), R_ES);

	int incdec = cpu.df? -DATA_BYTE : DATA_BYTE;

	cpu.esi += incdec;
	cpu.edi += incdec;
	
	print_asm("movs" str(SUFFIX) " %%ds:(%%esi),%%es:(%%edi)");

	return 1;
}

#include "cpu/exec/template-end.h"
