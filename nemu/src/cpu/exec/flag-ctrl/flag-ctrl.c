#include "cpu/exec/helper.h"

make_helper(std) {
	cpu.df = 1;
	print_asm("std");
	return 1;
}

make_helper(cld) {
	cpu.df = 0;
	print_asm("cld");
	return 1;
}
