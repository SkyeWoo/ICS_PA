#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"
#include "cpu/reg.h"

#define instr pop

make_helper(concat(pop_m_, SUFFIX)) {
	Operand addrm, addrr;
	
	read_ModR_M(cpu.eip + 1, &addrm, &addrr);
	
	if (ops_decoded.is_operand_size_16) {
		cpu.esp += 2;
		current_sreg = R_SS;
		swaddr_write(addrm.addr, 2, swaddr_read(cpu.esp, 2, current_sreg), current_sreg);
		//swaddr_write(addrm.addr, 2, swaddr_read(cpu.esp, 2));
	}
	else {
		cpu.esp += 4;
		current_sreg = R_SS;
		swaddr_write(addrm.addr, 4, swaddr_read(cpu.esp, 4, current_sreg), current_sreg);
	}
	print_asm_template1();

	return 1;
}

make_helper(concat(pop_r_, SUFFIX)) {
	int reg = instr_fetch(cpu.eip, 1) & 0x7;

	if (ops_decoded.is_operand_size_16) {
		reg_w(reg) = swaddr_read(cpu.esp, 2, R_SS);
		cpu.esp += 2;
	}
	else {
		reg_l(reg) = swaddr_read(cpu.esp, 4, R_SS);
		cpu.esp += 4;
	}

	print_asm("pop" str(SUFFIX) " %%%s", REG_NAME(reg));

	return 1;
}

/*
make_helper(concat(pop_a_, SUFFIX)) {
	int i = 7;
	for (; i >= 0; i--) {
#if DATA_BYTE == 2
		reg_w(i) = swaddr_read(cpu.esp, 2);
		cpu.esp += 2;
#elif DATA_BYTE == 4
		reg_l(i) = swaddr_read(cpu.esp, 4);
		cpu.esp += 4;
#endif
	}
#if DATA_BYTE == 2
	print_asm("popa");
#elif DATA_BYTE == 4
	print_asm("popad");
#endif

	return 1;
}

make_helper(concat(pop_f_, SUFFIX)) {
	cpu.eflags = swaddr_read(cpu.esp, DATA_BYTE);
	cpu.esp += DATA_BYTE;
#if DATA_BYTE == 2
	print_asm("popf");
#elif DATA_BYTE == 4
	print_asm("popfd");
#endif

	return 1;
}
*/

#include "cpu/exec/template-end.h"
