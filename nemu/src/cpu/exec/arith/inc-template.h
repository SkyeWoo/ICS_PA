#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;

	/* TODO: Update EFLAGS. */
	cpu.of = (!MSB(op_src->val) && MSB(result))? 1 : 0;
	cpu.af = (((op_src->val & 0x7) + 1) > 0x7)? 0 : 1;
	cpu.cf = ((long long)((long long)op_src->val + 1) >> (8 * DATA_BYTE)) & 1;

	OPERAND_W(op_src, result);

	cpu.sf = MSB(result);
	cpu.zf = !result;
	int count = 0;
	for (; result; count++) result &= result - 1;
	cpu.pf = (count % 2)? 1 : 0;

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
