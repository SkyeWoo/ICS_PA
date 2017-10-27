#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

#define instr movz

make_helper(concat(mov_zb_, SUFFIX)) {
	ModR_M m;
	m.val = instr_fetch(cpu.eip + 2, 1);

	if (m.mod == 3) {
		REG(m.reg) = (DATA_TYPE)(uint8_t)REG(m.R_M);

		print_asm("movzb" str(SUFFIX) " %%%s,%%%s", REG_NAME(m.R_M), REG_NAME(m.reg));
		return 2;
	}

	else {
		Operand addrm, addrr;
		addrm.size = DATA_BYTE;

		int len = read_ModR_M(cpu.eip + 2, &addrm, &addrr);
		REG(m.reg) = (DATA_TYPE)(uint8_t)MEM_R(addrm.addr);

		print_asm("movzb" str(SUFFIX) " %s,%%%s", addrm.str, REG_NAME(m.reg));
		
		return len + 1;
	}
}

make_helper(concat(mov_zw_, SUFFIX)) {
	ModR_M m;
	m.val = instr_fetch(cpu.eip + 2, 1);

	if (m.mod == 3) {
		REG(m.reg) = (uint32_t)(uint16_t)REG(m.R_M);

		print_asm("movzwl %%%s,%%%s", REG_NAME(m.R_M), REG_NAME(m.reg));
		return 2;
	}

	else {
		Operand addrm, addrr;
		addrm.size = 4;

		int len = read_ModR_M(cpu.eip + 2, &addrm, &addrr);
		REG(m.reg) = (uint32_t)(uint16_t)MEM_R(addrm.addr);

		print_asm("movzwl %s,%%%s", addrm.str, REG_NAME(m.reg));

		return len + 1;
	}
}
