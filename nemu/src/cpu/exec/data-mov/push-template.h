#include "cpu/exec/template-start.h"
#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "cpu/decode/operand.h"

#define instr push

static void do_execute() {
	if (ops_decoded.is_operand_size_16) {
		cpu.esp -= 2;
		swaddr_write(cpu.esp, 2, op_src->val, R_SS);
	}
	else {
		cpu.esp -= 4;
		swaddr_write(cpu.esp, 4, (int32_t)op_src->val, R_SS);
	}
	print_asm_template1();
}	

make_instr_helper(r)
make_instr_helper(i)

make_helper(concat(push_m_, SUFFIX)) {
	Operand addrm, addrr;
	addrm.size = DATA_BYTE;

	int len = read_ModR_M(cpu.eip + 1, &addrm, &addrr);
	if (ops_decoded.is_operand_size_16) {
		cpu.esp -= 2;
		swaddr_write(cpu.esp, 2, addrm.val, R_SS);
	}
	else {
		cpu.esp -= 4;
		swaddr_write(cpu.esp, 4, (int32_t)addrm.val, R_SS);
	}

	print_asm("push" str(SUFFIX) " %s", addrm.str); 

	return len + 1;
}

#include "cpu/exec/template-end.h"
