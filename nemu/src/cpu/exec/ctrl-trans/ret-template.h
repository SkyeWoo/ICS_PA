#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

#define instr ret

make_helper(concat(ret_near_, SUFFIX)) {
#if DATA_BYTE == 2
	cpu.eip = swaddr_read(cpu.esp, 2, R_SS) & 0xffff;
	cpu.esp += 2;
#else
	cpu.eip = swaddr_read(cpu.esp, 4, R_SS);
	cpu.esp += 4;
#endif

	print_asm("ret");

	/* since eip has been changed according to codes
	 * there is no need to return a displacement;
	 */
	return 0;
}

make_helper(concat(ret_i_, SUFFIX)) {
	uint16_t val = instr_fetch(cpu.eip + 1, 2);
	uint32_t addr;
#if DATA_BYTE == 1
	addr = swaddr_read(cpu.esp, 4, R_SS);
	cpu.esp += 4 + val;
#else
	addr = swaddr_read(cpu.esp, DATA_BYTE, R_SS);
	cpu.esp += DATA_BYTE + val;
#endif
	cpu.eip = addr - 3;

	print_asm("ret");

	return 3;
}

#include "cpu/exec/template-end.h"
