#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	DATA_TYPE temp = op_dest->val - op_src->val;

	cpu.cf = ((long long)((long long)op_dest->val - (long long)op_src->val) >> (8 * DATA_BYTE)) & 1;
	cpu.af = ((op_dest->val & 0x8) > (op_src->val & 0x8))? 0 : 1;
	cpu.zf = !temp;
	cpu.sf = MSB(temp);
	cpu.of = (MSB(op_dest->val) != MSB(op_src->val) && MSB(op_dest->val) != MSB(temp))? 1 : 0;
	int count = 0;
	for (; temp; count++) temp &= temp - 1;
	cpu.pf = (count % 2)? 1 : 0;

	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
