#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"
#include "nemu.h"

#define instr mov

TLB tlb[TLB_SIZE];
uint32_t swaddr_read(swaddr_t, size_t, uint8_t);
uint32_t lnaddr_read(lnaddr_t, size_t);
void init_tlb();

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(cpu.eip + 1, 4);
	current_sreg = R_DS;
	MEM_W(addr, REG(R_EAX));
	//swaddr_w(addr, DATA_BYTE, REG(R_EAX), R_DS); 

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(cpu.eip + 1, 4);
	current_sreg = R_DS;
	REG(R_EAX) = MEM_R(addr);
	//REG(R_EAX) = swaddr_r(addr, DATA_BYTE, R_DS);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

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

		REG(m.reg) = (DATA_TYPE)(uint8_t)addrm.val;
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
		REG(m.reg) = (uint32_t)(uint16_t)addrm.val;

		print_asm("movzwl %s,%%%s", addrm.str, REG_NAME(m.reg));

		return len + 1;
	}
}

make_helper(concat(mov_sb_, SUFFIX)) {
	ModR_M m;
	m.val = instr_fetch(cpu.eip + 2, 1);

	if (m.mod == 3) {
		REG(m.reg) = (DATA_TYPE_S)(int8_t)REG(m.R_M);

		print_asm("movsb" str(SUFFIX) " %%%s,%%%s", REG_NAME(m.R_M), REG_NAME(m.reg));
		return 2;
	}

	else {
		Operand addrm, addrr;
		addrm.size = DATA_BYTE;

		int len = read_ModR_M(cpu.eip + 2, &addrm, &addrr);
		REG(m.reg) = (DATA_TYPE_S)(int8_t)addrm.val;

		print_asm("movsb" str(SUFFIX) " %s,%%%s", addrm.str, REG_NAME(m.reg));
		return len + 1;
	}
}

make_helper(concat(mov_sw_, SUFFIX)) {
	ModR_M m;
	m.val = instr_fetch(cpu.eip + 2, 1);

	if (m.mod == 3) {
		REG(m.reg) = (int32_t)(int16_t)REG(m.R_M);

		print_asm("movswl %%%s,%%%s", REG_NAME(m.R_M), REG_NAME(m.reg));
		return 2;
	}

	else {
		Operand addrm, addrr;
		addrm.size = DATA_BYTE;

		int len = read_ModR_M(cpu.eip + 2, &addrm, &addrr);
		REG(m.reg) = (int32_t)(int16_t)addrm.val;

		print_asm("movswl %s,%%%s", addrm.str, REG_NAME(m.reg));
		return len + 1;
	}
}

#if DATA_BYTE == 4

make_helper(mov_cr2r) {
	uint8_t opcode = instr_fetch(cpu.eip + 2, 1);
	switch (opcode) {
		case 0xc0:
			cpu.eax = cpu.cr0.val;
			print_asm("mov %%cr0,%%%s", REG_NAME(R_EAX));
			break;
		case 0xd8:
			cpu.eax = cpu.cr3.val;
			print_asm("mov %%cr3, %%%s", REG_NAME(R_EAX));
			break;
	}

	return 2;
}

make_helper(mov_r2cr) {
	uint8_t opcode = instr_fetch(cpu.eip + 2, 1);
	switch (opcode) {
		case 0xc0:
			cpu.cr0.val = cpu.eax;
			print_asm("mov %%%s,%%cr0", REG_NAME(R_EAX));
			break;
		case 0xd8:
			cpu.cr3.val = cpu.eax;
			init_tlb();
			print_asm("mov %%%s,%%cr3", REG_NAME(R_EAX));
			break;
	}

	return 2;
}

#endif

#if DATA_BYTE == 2
make_helper(mov_rm2sreg) {
	uint8_t opcode = instr_fetch(cpu.eip + 1, 1);
	uint16_t temp = reg_w(R_EAX);
	switch (opcode) {
		case 0xd8:
			cpu.ds.selector = temp;
			sreg_load(R_DS);
			current_sreg = R_DS;
			print_asm("mov %%%s, ds", REG_NAME(R_EAX));
			break;
		case 0xc0:
			cpu.es.selector = temp;
			sreg_load(R_ES);
			current_sreg = R_ES;
			print_asm("mov %%%s, es", REG_NAME(R_EAX));
			break;
		case 0xd0:
			cpu.ss.selector = temp;
			sreg_load(R_SS);
			current_sreg = R_SS;
			print_asm("mov %%%s, ss", REG_NAME(R_EAX));
			break;
		default: break;
	}

	return 2;
}

#endif

#include "cpu/exec/template-end.h"
