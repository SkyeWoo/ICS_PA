#include "cpu/exec/helper.h"

make_helper(cdq) {
	if (ops_decoded.is_operand_size_16) {
		if ((int16_t)reg_w(0) < 0) reg_w(2) = 0xffff;
		else reg_w(2) = 0;

		print_asm("cwtl");
	}

	else {
		if ((int32_t)cpu.eax < 0) cpu.edx = 0xffffffff;
		else cpu.edx = 0;

		print_asm("cltd");
	}
		
	return 1;
}
