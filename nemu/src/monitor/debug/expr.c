#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include "elf.h"

extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;

enum {
	/* TODO: Add more token types */
	NOTYPE = 256, 
	EQ, NE, GT, LS, GE, LE,
	HEX, NUM, VAR, REG, 
	BIT_AND, BIT_OR, XOR, BIT_NOT,
	LOG_AND, LOG_OR, LOG_NOT,
	DEREF, NEG,
	LSHIFT, RSHIFT
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE},					// spaces
	{"\\(", '('},					// left parenthesis
	{"\\)", ')'},					// right parenthesis
	{"==", EQ},					// equal
	{"!=", NE},					// not equal
	{">=", GE},					// greater or equal
	{"<=", LE},					// less or equal
	{">", GT},					// greater
	{"<", LS},					// less
	{"\\+", '+'},					// plus
	{"-", '-'},					// minus
	{"\\*", '*'},					// multiply
	{"/", '/'},					// divide
	{"%", '%'},					// mod
	{"0[xX][0-9A-Fa-f]{1,}", HEX},			// hex number
	{"[0-9]{1,}", NUM},				// number
	{"[a-zA-Z0-9_]+", VAR},				// variable
	{"\\$e(ax|cx|dx|bx|sp|bp|si|di)", REG},		// register
	{"&{1}", BIT_AND},				// bitwise and
	{"\\|{1}", BIT_OR},				// bitwise or
	{"\\^", XOR},					// xor
	{"~", BIT_NOT},					// bitwise not
	{"&{2}", LOG_AND},				// logical and
	{"\\|{2}", LOG_OR},				// logical or
	{"!", LOG_NOT},					// logical not
	{"\\*", DEREF},					// dereference
	{"-", NEG},					// negative
	{"<{2}", LSHIFT},				// leftshift
	{">{2}", RSHIFT}				// rightshift

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		/* returns zero for a successful compilation or an error code for failure */
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {

		if (nr_token >= 32) {
			Log("Buffer overflow!"); return false;
		}

		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			/* regexec() returns zero for a successful match or REG_NOMATCH for failure */
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {

				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

//				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				memset(tokens[nr_token].str, '\0', 32);

				switch(rules[i].token_type) {

					case NOTYPE: break;

					case HEX: {

						if (substr_len >= 34) {
							panic("the substr is too long");
							return false;
						}

						/* copy substr without prefix '0x' */
						strncpy(tokens[nr_token].str, substr_start+2, substr_len-2);
						tokens[nr_token++].type = HEX;
						break;
					}
					
					case NUM: case VAR: case REG: {

						if (substr_len >= 32) {
							panic("the substr is too long");
							return false;
						}
						
						strncpy(tokens[nr_token].str, substr_start, substr_len);

						tokens[nr_token++].type = rules[i].token_type;
						
						break;

					}

					case '-': case '*': {
						if (nr_token == 0 || (tokens[nr_token-1].type != HEX && tokens[nr_token-1].type != NUM && tokens[nr_token-1].type != VAR && tokens[nr_token-1].type != REG && tokens[nr_token-1].type != ')')) {
							if (rules[i].token_type == '-') tokens[nr_token++].type = NEG;
							else tokens[nr_token++].type = DEREF;
						
							break;
						}

						else {
							tokens[nr_token++].type = rules[i].token_type;
							break;
						}
					}


					case '(': case ')': case EQ: case NE: 
					case GT: case LS: case GE: case LE: 
					case '+': case '/': case '%': 
					case BIT_AND: case BIT_OR: case XOR: case BIT_NOT:
					case LOG_AND: case LOG_OR: case LOG_NOT:
					case LSHIFT: case RSHIFT: {
						
						tokens[nr_token++].type = rules[i].token_type;
						break;
					}
					 
					default: panic("please implement me");
						assert(0);
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
		
	}

	return true; 
}

bool check_parentheses(int p, int q) {
	if (tokens[p].type != '(' || tokens[q].type != ')') return false;

	int i = p + 1, par = 0;
	for (; i < q; i++) {
		if (tokens[i].type == '(') par++;
		else if (tokens[i].type == ')') par--;

		if (par < 0) return false;
	}

	if (!par) return true;
	else return false;
}

int level(int type) {
	switch (type) {
		case BIT_NOT: case LOG_NOT:
		case DEREF: case NEG:
			return 11;
		case '*': case '/': case '%':
			return 10;
		case '+': case '-':
			return 9;
		case LSHIFT: case RSHIFT:
			return 8;
		case GT: case LS: case GE: case LE:
			return 7;
		case EQ: case NE:
			return 6;
		case BIT_AND:
			return 5;
		case XOR:
			return 4;
		case BIT_OR:
			return 3;
		case LOG_AND:
			return 2;
		case LOG_OR:
			return 1;
		default: assert(0);
	}
}

int dominant_op(int p, int q) {
	/* if parentheses are not matched return -1 */
	int i = p, par = 0;
	for (; i <= q; i++) {
		if (tokens[i].type == '(') par++;
		else if (tokens[i].type == ')') par--;

		if (par < 0) return -1;
	}
	if (par) return -1;

	int index = -1, min = ~(1<<31);
	for (i = p, par = 0; i <= q; i++) {
		if (tokens[i].type == '(') par ++;
		else if (tokens[i].type == ')') par--;

		/* parenthesis or inside matched parentheses or not an operator just go next loop */
		if ((par != 0) || (tokens[i].type == '(') || (tokens[i].type == ')') || (tokens[i].type == HEX) || (tokens[i].type == NUM) || (tokens[i].type == VAR) || (tokens[i].type == REG)) continue;

		/* find the dominant op */
		int temp = level(tokens[i].type);
		if (temp <= min) {
			min = temp; index = i;
		}
	}

	return index;
}

uint32_t eval(int p, int q, bool *success) {
	if (p > q) {
		/* Bad expression */
		*success = false;
		return 0;
	}

	else if (p == q) {
		/* Single token
		 * For now this token should be a number.
		 * Return the value of the number.
		 */
		uint32_t addr;
		switch(tokens[p].type) { 
			case HEX: {
				sscanf(tokens[p].str, "%x", &addr);
				return addr;
				  }

			case NUM: {
				return atoi(tokens[p].str);
			}

			case VAR: {
				int temp;
				for (temp = 0; temp < nr_symtab_entry; temp++) {
					if (!strcmp(&strtab[symtab[temp].st_name], tokens[p].str))
						return symtab[temp].st_value;
				}

				*success = false; return 0;
			}
			

			case REG: {
				if (!strcmp(tokens[p].str, "$eax")) return cpu.eax;
				else if (!strcmp(tokens[p].str, "$ecx")) return cpu.ecx;
				else if (!strcmp(tokens[p].str, "$edx")) return cpu.edx;
				else if (!strcmp(tokens[p].str, "$ebx")) return cpu.ebx;
				else if (!strcmp(tokens[p].str, "$esp")) return cpu.esp;
				else if (!strcmp(tokens[p].str, "$ebp")) return cpu.ebp;
				else if (!strcmp(tokens[p].str, "$esi")) return cpu.esi;
				else if (!strcmp(tokens[p].str, "$edi")) return cpu.edi;
				else {
					*success = false; return 0;
				}
			}

			default: *success = false; return 0;
		}
				
	}

	else if (check_parentheses(p, q) == true) {
		/* The expression is surrounded by a matched pair of parentheses.
		 * If that is the case, just throw away the parentheses.
		 */
		return eval(p + 1, q - 1, success);
	}
	
	else {
		/* We should do more things here. */
		int op = dominant_op(p, q);
		if (op == -1) {
			*success = false; return 0;
		}

		uint32_t val2 = eval(op+1, q, success);
		switch (tokens[op].type) {
			case BIT_NOT: return ~val2;
			case LOG_NOT: return !val2;
			case DEREF: return(swaddr_read(val2, 4, R_DS));
			case NEG: return -val2; 
			default: assert(op != p);
		}

		uint32_t val1 = eval(p, op-1, success);
		switch (tokens[op].type) {
			case EQ: return val1 == val2;
			case NE: return val1 != val2;
			case GT: return val1 > val2;
			case LS: return val1 < val2;
			case GE: return val1 >= val2;
			case LE: return val1 <= val2;

			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': return val1 / val2;
			case '%': return val1 % val2;

			case BIT_AND: return val1 & val2;
			case BIT_OR: return val1 | val2;
			case XOR: return val1 ^ val2;

			case LOG_AND: return val1 && val2;
			case LOG_OR: return val1 || val2;
			case LSHIFT: return val1 << val2;
			case RSHIFT: return val1 >> val2;

			default : return 0;
		}

		printf("Invalid expression!\n");
		*success = false; assert(0); return 0;
	}

	return 0;
}

uint32_t expr(char *e, bool *success) {
	*success = true;

	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	return eval(0, nr_token-1, success);
}
