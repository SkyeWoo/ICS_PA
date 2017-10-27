#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "memory/memory.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>

extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;

void cpu_exec(uint32_t);
hwaddr_t page_translate(lnaddr_t);
/* ~/ics2016/nemu/src/monitor/cpu-exec.c */

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);
static int info_r();
static int info_w();

static int cmd_p(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);
static int cmd_d(char *args);

int find_func_name(int eip);
static int cmd_bt(char *args);

static int cmd_page(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	{ "si", "Step assembly", cmd_si },
	{ "info", "List infomation like registers in use or breakpoints", cmd_info },
	{ "p", "Evaluate the expression", cmd_p},
	{ "x", "Print memory pointed to by address", cmd_x },
	{ "w", "Set a new watchpoint", cmd_w},
	{ "d", "Delete a watchpoint", cmd_d},
	{ "bt", "Print stack frame", cmd_bt},
	{ "page", "Print the result of ln2pa", cmd_page}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args) {
	uint32_t n;

	if(args == NULL)
		/* no argument given */
		cpu_exec(1);	
	else {
		if (sscanf(args, "%u", &n) == 0) {
			cpu_exec(1); return 0;
		}

		cpu_exec(n);
	}

	return 0;	
}

static int cmd_info(char *args) {
	if (args == NULL) {
		/* no argument given */
		printf("\"info\" must be followed by the name of an info command.\n");
		printf("List of info subcommands:\n\n");
		printf("info r -- List of integer registers and their contents\n");
		printf("info w -- Status of specified watchpoints (all watchpoints if no argument)\n");
	}
	else {
		args = strtok(NULL, " ");
		if (strcmp(args, "r") == 0) return info_r();
		else if (strcmp(args, "w") == 0) return info_w();

		/* invalid argument */	
		printf("Undefined info command: \"%s\".  Try \"help info\".\n", args);
	}

	return 0;
}

static int info_r() {
	int n1, n2, n3;

	/*GPRs*/
	if (cpu.eax) n1 = 10; else n1 = 1;
	printf("eax\t%#0*x\t%d\n", n1, cpu.eax, cpu.eax);
	if (cpu.ecx) n1 = 10; else n1 = 1;
	printf("ecx\t%#0*x\t%d\n", n1, cpu.ecx, cpu.ecx);
	if (cpu.edx) n1 = 10; else n1 = 1;
	printf("edx\t%#0*x\t%d\n", n1, cpu.edx, cpu.edx);
	if (cpu.ebx) n1 = 10; else n1 = 1;
	printf("ebx\t%#0*x\t%d\n", n1, cpu.ebx, cpu.ebx);
	if (cpu.esp) n1 = 10; else n1 = 1;
	printf("esp\t%#0*x\t%d\n", n1, cpu.esp, cpu.esp);
	if (cpu.ebp) n1 = 10; else n1 = 1;
	printf("ebp\t%#0*x\t%d\n", n1, cpu.ebp, cpu.ebp);
	if (cpu.esi) n1 = 10; else n1 = 1;
	printf("esi\t%#0*x\t%d\n", n1, cpu.esi, cpu.esi);
	if (cpu.edi) n1 = 10; else n1 = 1;
	printf("edi\t%#0*x\t%d\n", n1, cpu.edi, cpu.edi);

	/*SRs*/
	char name[6][3] = {"cs", "ss", "ds", "es", "fs", "gs"};
	int i;
	for (i = 0; i < 6; i++) {
		if (cpu.sr[i].selector) n1 = 10; else n1 = 1;
		if (cpu.sr[i].limit) n2 = 10; else n2 = 1;
		if (cpu.sr[i].base) n3 = 10; else n3 = 1;
		printf("%s: selector = %#0*x\tlimit = %#0*x\tbase = %#0*x\n", name[i], n1, cpu.sr[i].selector, n2, cpu.sr[i].limit, n3, cpu.sr[i].base);
	}

	/*EFLAGS*/
	if (cpu.eflags) n1 = 10; else n1 = 1;
	printf("eflags %#0*x\t%d\t", n1, cpu.eflags, cpu.eflags);
	printf("[ ");
	if (cpu.cf) printf("CF ");
	if (cpu.zf) printf("ZF ");
	if (cpu.sf) printf("SF ");
	if (cpu.of) printf("OF ");
	printf("]\n");

	/*EIP*/
	if (cpu.eip) n1 = 10; else n1 = 1;
	printf("eip\t%#0*x\n", n1, cpu.eip);

	/*LDTR, IDTR, GDTR*/
/*	if (cpu.ldtr.limit) n1 = 10; else n1 = 1;
	if (cpu.ldtr.base) n2 = 10; else n2 = 1;
	printf("ldtr:\tlimit = %#0*x\tbase = %#0*x\n", n1, cpu.ldtr.limit, n2, cpu.ldtr.base);
	if (cpu.idtr.limit) n1 = 10; else n1 = 1;
	if (cpu.idtr.base) n2 = 10; else n2 = 1;
	printf("idtr:\tlimit = %#0*x\tbase = %#0*x\n", n1, cpu.idtr.limit, n2, cpu.idtr.base);
*/	if (cpu.gdtr.limit) n1 = 10; else n1 = 1;
	if (cpu.gdtr.base) n2 = 10; else n2 = 1;
	printf("gdtr:\tlimit = %#0*x\tbase = %#0*x\n", n1, cpu.gdtr.limit, n2, cpu.gdtr.base);

	/*CR0, CR3*/
	if (cpu.cr0.val) n1 = 10; else n1 = 1;
	if (cpu.cr3.val) n2 = 10; else n2 = 1;
	printf("cr0: %#0*x\tcr3: %#0*x\n", n1, cpu.cr0.val, n2, cpu.cr3.val);
	return 0;
}

static int info_w() {
	print_wp();
	return 0;
}

static int cmd_p(char *args) {
	if (!args) {
		printf("Expression required.\n");
		return 0;
	}

	bool success;
	uint32_t result = expr(args, &success);

	if (success) printf("0x%x\t%u\n", result, result);
	else printf("Bad expression\n");

	return 0;
}

static int cmd_x(char *args) {
	if (!args) {
		printf("Argument required (starting display address).\n");
		return 0;
	}
	
	/* extract the first argument */

	while (args && *args && *args == ' ') args++;
	
	int num = 0;
	while (args && *args && isdigit(*args)) {
		num = num * 10 + *args - 48;
		args++;
	}
	
	/* acquiesce to output one order */
	if (!num) {
		num = 1;
		if (*args == 'x' || *args == 'X') args--;
	}


	while (args && *args && *args == ' ') args++;

	if (!args || !*args) {
		printf("Expression required.\n");
		return 0;
	}

	/* analyse the expression */
	bool success;
	uint32_t addr = expr(args, &success);
	/* ~/ics2016/nemu/src/monitor/debug/expr.c */

	if (success) {
		int i = 0;
		for (; i < num; i++) {
			int j = 1, k = swaddr_read(addr, 4, R_DS);
			if (k) j = 10;
			printf("%#0*x:\t%#0*x\n", 10, addr, j, k);
			addr += 4;
		}
	}	
	else {
		printf("%s:\tCannot access memory at address %s\n", args, args);
	}

	return 0;
}

static int cmd_w(char *args) {
	if (!args) {
		printf("Argument required (expression to compute).\n");
		return 0;
	}

	bool success;
	uint32_t result = expr(args, &success);

	if (!success) printf("Wrong expression!\n");
	else {
		WP *new = new_wp();
		if (!new) { printf("watchpoint create error!\n"); return 0; }

		new->value = result;
		new->expr = (char*)malloc(strlen(args));
		strcpy(new->expr, args);
	}

	return 0;
}

static int cmd_d(char *args) {
	if (!args) {
		printf("Argument required.\n");
		return 0;
	}

	int no;
	if (sscanf(args, "%d", &no) != 1) {
		printf("Watchpoint's number error!\n");
		return 0;
	}

	WP *wp = found_wp(no);
	if (wp) free_wp(wp);
	else printf("No watchpoint number %d", no);

	return 0;
}

typedef struct {
	swaddr_t prev_ebp;
	swaddr_t ret_addr;
	uint32_t args[4];
} PartOfStackFrame;

int find_func_name(int eip) {
	int i = 0;
	for (; i < nr_symtab_entry; i++)
		if ((symtab[i].st_info & STT_FUNC) && (eip >= symtab[i].st_value) && (eip <= symtab[i].st_value + symtab[i].st_size))
			return symtab[i].st_name;
	
	Assert(0, "function not find");
}

static int cmd_bt(char *args) {
	uint32_t ebp = cpu.ebp, eip = cpu.eip;
	if (eip <= 0x100000) { printf("No Stack!\n"); return 0;}	
	while (ebp) {
		PartOfStackFrame sf;
		sf.prev_ebp = swaddr_read(ebp, 4, R_SS);
		sf.ret_addr = swaddr_read(ebp + 4, 4, R_SS);
		printf("ebp: 0x%x, funcname: %s\n", ebp, strtab + find_func_name(eip));
		printf("prev_ebp: 0x%x, ret_addr: 0x%x\n", sf.prev_ebp, sf.ret_addr);
		
		int i = 0;
		for (; i < 4; i++) {
			if ((ebp + 12 + i * 4) <= 0x8000000) {
				sf.args[i] = swaddr_read(ebp + 8 + i * 4, 4, R_SS);
				printf("args[%d]: 0x%x\t ", i, sf.args[i]);
			}
			else break;
		}
		putchar('\n');
		ebp = sf.prev_ebp; eip = sf.ret_addr;
	}

	return 0;
}

static int cmd_page(char *args) {
	if (!args) {
		printf("Address required.\n");
		return 0;
	}

	bool success;
	uint32_t result = expr(args, &success);

	if (!success) printf("Not a valid address!\n");
	else {
		hwaddr_t hwaddr;
		if (cpu.cr0.pg && cpu.cr0.pe) hwaddr = page_translate(result);
		else {
			printf("Not in page mode!\n");
			hwaddr = result;
		}

		printf("0x%x\n", hwaddr);
	}

	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
