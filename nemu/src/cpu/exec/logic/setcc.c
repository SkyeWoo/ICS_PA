#define DATA_BYTE 1

#include "cpu/exec/helper.h"
#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

make_helper(setne_rm_b) {
	ModR_M m;
	m.val = instr_fetch(cpu.eip + 2, 1);

	if (m.mod == 3) {
		REG(m.R_M) = !cpu.zf;

		print_asm("setne" " %%%s", REG_NAME(m.R_M));

		return 2;
	}	

	else {
		Operand addrm, addrr;
		addrm.size = DATA_BYTE;

		int len = read_ModR_M(cpu.eip + 2, &addrm, &addrr);

		MEM_W(addrm.addr, !cpu.zf);

		print_asm("setne %s", addrm.str);
		return len + 1;
	}
}

make_helper(sete_rm_b) {
	ModR_M m;
	m.val = instr_fetch(cpu.eip + 2, 1);

	if (m.mod == 3) {
		REG(m.R_M) = cpu.zf;

		print_asm("sete" " %%%s", REG_NAME(m.R_M));

		return 2;
	}

	else {
		Operand addrm, addrr;
		addrm.size = DATA_BYTE;

		int len = read_ModR_M(cpu.eip + 2, &addrm, &addrr);

		MEM_W(addrm.addr, cpu.zf);

		print_asm("sete %s", addrm.str);
		return len + 1;
	}
}
