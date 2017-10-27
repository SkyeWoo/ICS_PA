#include "nemu.h"
#include "common.h"
#include <stdlib.h>
#include "burst.h"

TLB tlb[TLB_SIZE];

hwaddr_t page_translate(lnaddr_t lnaddr);

hwaddr_t tlb_translate(lnaddr_t lnaddr) {
	uint32_t tag = (lnaddr >> 12) & 0xfffff;
	int i = 0;
	for (; i < TLB_SIZE; i++) {
		if (tlb[i].valid && tlb[i].tag == tag) {
			return (tlb[i].paddr | (lnaddr & 0xfff));}
	}

	/* no hit */
	hwaddr_t hwaddr = page_translate(lnaddr);
	for (i = 0; i < TLB_SIZE; i++)
		if (!tlb[i].valid) break;
	// get a tlb page in random
	if (i == TLB_SIZE) i = rand() & (TLB_SIZE - 1);
	tlb[i].valid = true;
	tlb[i].tag = tag;
	tlb[i].paddr = hwaddr & 0xfffff000;
	return hwaddr;
}

void init_tlb() {
	int i = 0;
	for (; i < TLB_SIZE; i++) tlb[i].valid = false;
}
