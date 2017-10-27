#include "cpu/exec/template-start.h"

uint32_t dram_read(hwaddr_t, size_t);

#define instr lgdt

static void do_execute() {
	cpu.gdtr.limit = dram_read(op_src->addr, 2);
	//cpu.gdtr.limit = swaddr_read(op_src->addr, 2, sreg);
	if (ops_decoded.is_operand_size_16)
		//cpu.gdtr.base = swaddr_read(op_src->addr + 2, 3, sreg);
		cpu.gdtr.base = dram_read(op_src->addr + 2, 3);
	else //cpu.gdtr.base = swaddr_read(op_src->addr + 2, 4, sreg);
		cpu.gdtr.base = dram_read(op_src->addr + 2, 4);

	print_asm_template1();
}

make_instr_helper(rm);

#include "cpu/exec/template-end.h"
