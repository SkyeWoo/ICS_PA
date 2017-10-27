#include "cpu/exec/template-start.h"
#include "cpu/decode/modrm.h"

uint32_t dram_read(hwaddr_t, size_t);

#define instr jmp

make_helper(concat(jmp_rel_, SUFFIX)) {
	DATA_TYPE temp = instr_fetch(cpu.eip + 1, DATA_BYTE);
	if (ops_decoded.is_operand_size_16) cpu.eip = (cpu.eip + (DATA_TYPE_S)temp) & 0xffff;
	else cpu.eip += (DATA_TYPE_S)temp;
	
	print_asm("jmp %x", cpu.eip + 1 + DATA_BYTE);

	return DATA_BYTE + 1;
}

make_helper(concat(jmp_rm_, SUFFIX)) {
	Operand addrm, addrr;
	addrm.size = DATA_BYTE;
	read_ModR_M(cpu.eip + 1, &addrm, &addrr);
	
	if (ops_decoded.is_operand_size_16) cpu.eip = addrm.val & 0xffff;
	else cpu.eip = addrm.val;

	print_asm("jmp %x", cpu.eip);

	/* since eip has been changed according to codes
	 * there is no need to add a displacement
	 */
	return 0;
}

#if DATA_BYTE == 4

make_helper(ljmp) {
	uint32_t eip_temp = instr_fetch(cpu.eip + 1, 4);
	cpu.cs.selector = instr_fetch(cpu.eip + 5, 2);
	uint16_t index = ((cpu.cs.selector >> 3) & 0x1fff);
	cpu.eip = eip_temp;	

	cpu.cs.hidden = (((uint64_t)dram_read(cpu.gdtr.base + (index << 3) + 4, 4)) << 32) + dram_read(cpu.gdtr.base + (index << 3), 4);

	print_asm("ljmp" " $0x%x, $0x%x", cpu.cs.selector, cpu.eip);

	return 0;
}

#endif

#include "cpu/exec/template-end.h"
