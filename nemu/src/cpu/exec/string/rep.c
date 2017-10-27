#include "cpu/exec/helper.h"

make_helper(exec);

make_helper(rep) {
	int len;
	int count = 0;
	
	if(instr_fetch(eip + 1, 1) == 0xc3) {
		/* repz ret */
		exec(eip + 1);
		printf("here!\n");
		len = -1;
	}
	else {
		while(cpu.ecx) {
			exec(eip + 1);
			count ++;
			cpu.ecx --;
			assert(ops_decoded.opcode == 0xa4	// movsb
				|| ops_decoded.opcode == 0xa5	// movsw
				|| ops_decoded.opcode == 0xaa	// stosb
				|| ops_decoded.opcode == 0xab	// stosw
				|| ops_decoded.opcode == 0xa6	// cmpsb
				|| ops_decoded.opcode == 0xa7	// cmpsw
				|| ops_decoded.opcode == 0xae	// scasb
				|| ops_decoded.opcode == 0xaf	// scasw
				);

			/* TODO: Jump out of the while loop if necessary. */
			uint8_t prim = instr_fetch(eip + 1, 1);
			
			if (prim == 0xa6 || (prim == 0xa6 && ops_decoded.is_operand_size_16) || prim == 0xae || (prim == 0xaf && ops_decoded.is_operand_size_16))
				if (!cpu.zf) break;
		}
		len = 1;
	}

#ifdef DEBUG
	char temp[80];
	sprintf(temp, "rep %s", assembly);
	sprintf(assembly, "%s[cnt = %d]", temp, count);
#endif
	
	return len + 1;
}

make_helper(repnz) {
	int count = 0;

	while(cpu.ecx) {
		exec(eip + 1);
		count ++;
		cpu.ecx --;
		assert(ops_decoded.opcode == 0xa6	// cmpsb
				|| ops_decoded.opcode == 0xa7	// cmpsw
				|| ops_decoded.opcode == 0xae	// scasb
				|| ops_decoded.opcode == 0xaf	// scasw
			  );
		
		/* TODO: Jump out of the while loop if necessary. */
		uint8_t prim = instr_fetch(eip + 1, 1);
			
		if (prim == 0xa6 || (prim == 0xa6 && ops_decoded.is_operand_size_16) || prim == 0xae || (prim == 0xaf && ops_decoded.is_operand_size_16))
			if (cpu.zf) break;
	}

#ifdef DEBUG
	char temp[80];
	sprintf(temp, "repnz %s", assembly);
	sprintf(assembly, "%s[cnt = %d]", temp, count);
#endif

	return 1 + 1;
}
