#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp() {
	if (!free_) {
		printf("No enough watchpoints!\n");
	       	assert(0);
	}

	/* insert the node at the beginning of the list */
	WP *p = free_;
	free_ = p->next;
	p->next = head;
	p->type = WATCHPOINT;
	p->expr = NULL;
	head = p;
	
	return p;
}

void free_wp(WP *wp) {
	if (!head) {
		printf("Free error!\n");
		return;
	}

	free(wp->expr);
	wp->expr = NULL;

	if (head == wp) {
		head = wp->next;
		wp->next = free_;
		free_ = wp;
	}

	else {
		WP *new = head;
		while (new->next != wp) {
			Assert(new->next, "Free error! No found watchpoint!\n");
			new = new->next;
		}

		new->next = wp->next;
		wp->next = free_;
		free_ = wp;
	}

	return;
}

void print_wp() {
	if (!head) {
		printf("No watchpoints!\n");
		return;
	}

	WP *wp = head;

	for (; wp; wp = wp->next) {
		printf("Hardware watchpoint %d: %s\n\n", wp->NO, wp->expr);
		printf("value = %d\n", wp->value);
	}
}

WP *found_wp(int no) {
	WP *wp = head;

	while (wp) {
		if (wp->NO == no) return wp;
		wp = wp->next;
	}

	return NULL;
}

void calc_wp(WP *wp) {
	Assert(wp->expr, "No expression in watchpoint!\n");
	bool success;
	wp->value = expr(wp->expr, &success);
	Assert(success, "Bad expression in watchpoint!\n");
	return;
}

bool change_wp(WP **wp, uint32_t *oldv, uint32_t *newv) {
	WP *temp = head;
	while (temp) {
		uint32_t old_value = temp->value;
		calc_wp(temp);
		uint32_t new_value = temp->value;

		if (temp->type == WATCHPOINT) {
			if (old_value != new_value) {
				if (wp) *wp = temp;
				if (oldv) *oldv = old_value;
				if (newv) *newv = new_value;
				return true;
			}
		}

		temp = temp->next;
	}

	return false;
}
