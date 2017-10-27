#include "cpu/exec/template-start.h"

#define instr je

make_helper(concat(je_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (cpu.zf) {
		cpu.eip += temp;
		print_asm("je %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}
	
	else print_asm("je %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jne

make_helper(concat(jne_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (!cpu.zf) {
		cpu.eip += temp;
		print_asm("jne %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jne %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jae

make_helper(concat(jae_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif
	if (!cpu.cf) {
		cpu.eip += temp;
		print_asm("jae %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jae %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jbe

make_helper(concat(jbe_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (cpu.zf || cpu.cf) {
		cpu.eip += temp;
		print_asm("jbe %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jbe %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jl

make_helper(concat(jl_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (cpu.sf ^ cpu.of) {
		cpu.eip += temp;
		print_asm("jl %x", cpu.eip + opsize + DATA_BYTE);	
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jl %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jle

make_helper(concat(jle_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (cpu.zf || (cpu.sf ^ cpu.of)) {
		cpu.eip += temp;
		print_asm("jle %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jle %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jg

make_helper(concat(jg_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (!cpu.zf && (cpu.sf == cpu.of)) {
		cpu.eip += temp;
		print_asm("jg %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jg %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jge

make_helper(concat(jge_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (cpu.sf == cpu.of) {
		cpu.eip += temp;
		print_asm("jge %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jge %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr ja

make_helper(concat(ja_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (!cpu.cf && !cpu.zf) {
		cpu.eip += temp;
		print_asm("ja %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("ja %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jb

make_helper(concat(jb_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (cpu.cf) {
		cpu.eip += temp;
		print_asm("jb %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jb %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr js

make_helper(concat(js_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif
	if (cpu.sf) {
		cpu.eip += temp;
		print_asm("js %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("js %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#define instr jns

make_helper(concat(jns_rel_, SUFFIX)) {
	DATA_TYPE_S temp;
	int opsize;
#if DATA_BYTE == 1
	temp = instr_fetch(cpu.eip + 1, 1);
	opsize = 1;
#else
	temp = instr_fetch(cpu.eip + 2, DATA_BYTE);
	opsize = 2;
#endif

	if (!cpu.sf) {
		cpu.eip += temp;
		print_asm("jns %x", cpu.eip + opsize + DATA_BYTE);
		if (ops_decoded.is_operand_size_16) cpu.eip &= 0xffff;
	}

	else print_asm("jns %x", cpu.eip + temp + opsize + DATA_BYTE);

	return DATA_BYTE + 1;
}

#undef instr

#include "cpu/exec/template-end.h"
