#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat(leave_, SUFFIX)) {
	cpu.esp = cpu.ebp;

	/* ebp's index is 5 */
	if (ops_decoded.is_operand_size_16) {
		reg_w(5) = swaddr_read(cpu.esp, 2, R_SS);
		cpu.esp += 2;
	}
	else {
		reg_l(5) = swaddr_read(cpu.esp, 4, R_SS);
		cpu.esp += 4;
	}

	print_asm("leave");

	return 1;
}

#include "cpu/exec/template-end.h"
