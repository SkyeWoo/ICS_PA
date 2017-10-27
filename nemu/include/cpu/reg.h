#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };
enum { R_ES, R_CS, R_SS, R_DS, R_FS, R_GS };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

/* the structure of segment registers */
struct seg_reg {
	uint16_t selector;
	uint32_t limit;
	uint32_t base;
	uint32_t hidden;
};

typedef struct {
	
	/*GPRs*/
	union {
		union {
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		} gpr[8];

		struct {
			/* Do NOT change the order of the GPRs' definitions. */
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};
	
	/*SRs*/
	union {
		struct seg_reg sr[6];

		struct {
			/* Do NOT change the order of the SRs' definitions. */
			struct seg_reg es, cs, ss, ds, fs, gs;
			/* fs & gs are not used */
		};
	};

	/*EFALGS*/
	union {
		uint32_t eflags;

		struct {
			uint8_t cf	:1;	//CARRY FLAG
			uint8_t rsv_1	:1;
			uint8_t pf	:1;	//PARITY FLAG
			uint8_t rsv_2	:1;
			uint8_t af	:1;	//AUXILIARY CARRY
			uint8_t rsv_3	:1;
			uint8_t zf	:1;	//ZERO FLAG
			uint8_t sf	:1;	//SIGN FLAG
			uint8_t tf	:1;	//TRAP FLAG
			uint8_t itf	:1;	//INTERRUPT ENABLE
       			uint8_t df	:1;	//DIRECTION FLAG
			uint8_t of	:1;	//OVERFLOW
			uint8_t iopl	:1;	//I/O PRIVILEGE LEVEl
			uint8_t nt	:1;	//NESTED TASK FLAG
			uint8_t rsv_4	:1;
			uint8_t rf	:1;	//RESUME FLAG
			uint8_t vm	:1;	//VIRTUAL 8086 MODE
		};
	};

	/*EIP*/
	swaddr_t eip;

	/*GDTR*/
	struct {
		uint16_t limit;
		lnaddr_t base;
	}gdtr;

	/*CR0*/
	union {
		uint32_t val;

		struct {
			uint8_t pe	:1;	//PROTECTION ENABLE
			uint8_t mp	:1;	//MATH PRESENT
			uint8_t em	:1;	//EMULATION
			uint8_t ts	:1;	//TASK SWITCHED
			uint8_t et	:1;	//TXTENSION TYPE
			uint32_t rsv_5	:26;
			uint8_t pg	:1;	//PAGING
		};
	}cr0;

	/*CR2*/
	//lnaddr_t cr2;

	/*CR3*/
	union {
		uint32_t val;

		struct {
			uint16_t rsv_6	:12;
			uint32_t pdbr	:20;	//PAGE DIRECTORY BASE REGISTER
		};
	}cr3;
	
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

void sreg_load(uint8_t sreg);
uint8_t current_sreg;

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
