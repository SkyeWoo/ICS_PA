#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

enum { WATCHPOINT, BREAKPOINT };

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */

	int type;
	uint32_t value;
	char *expr;
} WP;

void init_wp_pool();

WP *new_wp();

void free_wp(WP *wp);

void print_wp();

WP *found_wp(int no);

void calc_wp(WP *wp);

bool change_wp(WP **wp, uint32_t *oldv, uint32_t *newv);

#endif
