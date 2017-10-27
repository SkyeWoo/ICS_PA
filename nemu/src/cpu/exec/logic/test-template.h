#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
	DATA_TYPE temp = op_src->val & op_dest->val;

	cpu.cf = 0; cpu.of = 0;
	cpu.zf = !temp;
	cpu.sf = (MSB(temp))? 1 : 0;
	int count = 0;
	for (; temp; count++) temp &= temp - 1;
	cpu.pf = (count % 2) ? 1 : 0;
	
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
