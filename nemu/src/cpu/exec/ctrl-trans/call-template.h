#include "cpu/exec/template-start.h"
#include "stdlib.h"

#define instr call

make_helper(concat(call_rel_, SUFFIX)) {
	DATA_TYPE temp = instr_fetch(cpu.eip + 1, DATA_BYTE);
	cpu.esp -= DATA_BYTE;
	swaddr_write(cpu.esp, DATA_BYTE, cpu.eip + DATA_BYTE + 1, R_SS);

	if (ops_decoded.is_operand_size_16) cpu.eip = (cpu.eip + (DATA_TYPE_S)temp) & 0xffff;
	else cpu.eip += (DATA_TYPE_S)temp;

	print_asm("call %x", cpu.eip + DATA_BYTE + 1);

	return DATA_BYTE + 1; // "1" for opcode
}

make_helper(concat(call_rm_, SUFFIX)) {
	int len = concat(decode_rm_, SUFFIX)(cpu.eip + 1);
	cpu.esp -= DATA_BYTE;
	swaddr_write(cpu.esp, DATA_BYTE, cpu.eip + len + 1, R_SS);
	
	DATA_TYPE_S dis = op_src->val;

	print_asm("call %s", op_src->str);
	cpu.eip = dis - len - 1;
#if DATA_BYTE == 2
	cpu.eip &= 0xffff;
#endif

	return len + 1;
}

#include "cpu/exec/template-end.h"
