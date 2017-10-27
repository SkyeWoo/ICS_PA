#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "pop-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "pop-template.h"
#undef DATA_BYTE

make_helper_v(pop_r)
make_helper_v(pop_m)
//make_helper_v(pop_a)
//make_helper_v(pop_f)

make_helper(pop_ds) {
	assert(0); return 1;
}

make_helper(pop_es) {
	assert(0); return 1;
}

make_helper(pop_ss) {
	assert(0); return 1;
}

make_helper(pop_fs) {
	assert(0); return 1;
}

make_helper(pop_gs) {
	assert(0); return 1;
}


