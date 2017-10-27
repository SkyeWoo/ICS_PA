#include "cpu/exec/template-start.h"

#define instr add

static void do_execute() {
	DATA_TYPE temp = op_src->val + op_dest->val;

	cpu.of = ((MSB(op_src->val) == MSB(op_dest->val)) && (MSB(op_src->val) != MSB(temp)))? 1 : 0;
	cpu.af = ((op_dest->val & 0x7) + (op_src->val & 0x7) > 0x7)? 0 : 1;
	cpu.cf = ((long long)((long long)op_dest->val + (long long)op_src->val) >> (8 * DATA_BYTE)) & 1;
	cpu.sf = MSB(temp);
	cpu.zf = !temp;
	
	OPERAND_W(op_dest, temp);
	
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
