#include "cpu/exec/template-start.h"

#define instr sbb

static void do_execute() {
	DATA_TYPE temp;
	if (op_src->size == 1 && (op_dest->size == 2 || op_dest->size == 4)) {
		temp = op_src->val << (DATA_BYTE * 8 - 8);
		temp = ((int)temp >> (DATA_BYTE * 8 - 8)) + cpu.cf;
	}
	else temp = op_src->val + cpu.cf;

	cpu.of = (MSB(op_dest->val) != MSB(op_src->val) && MSB(op_dest->val) != MSB(op_dest->val - temp))? 1 : 0;
	cpu.af = ((op_dest->val & 0x8) > (op_src->val & 0x8))? 0 : 1;
	cpu.cf = ((long long)((long long)op_dest->val - (long long)temp) >> (8 * DATA_BYTE)) & 1;

	OPERAND_W(op_dest, op_dest->val - temp);

	cpu.sf = MSB(op_dest->val);
	cpu.zf = !op_dest->val;
	int count = 0;
	temp = op_dest->val;
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
