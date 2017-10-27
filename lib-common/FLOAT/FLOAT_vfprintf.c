#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include "FLOAT.h"

extern char _vfprintf_internal;
extern char _fpmaxtostr;
extern char _ppfs_setargs;
extern int __stdio_fwrite(char *buf, int len, FILE *stream);

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
	/* TODO: Format a FLOAT argument `f' and write the formating
	 * result to `stream'. Keep the precision of the formating
	 * result with 6 by truncating. For example:
	 *              f          result
	 *         0x00010000    "1.000000"
	 *         0x00013333    "1.199996"
	 */
	
	char buf[80];
	int len;
	int sign = (((f >> 31) & 1) == 0)? 0 : 1;
	if (sign) f = -f;
	
	int integer = f >> 16;
	int frac = f & 0xffff, i = 0;
	frac = (int)(((long long)frac * 1000000) >> 16);
	
	if (!sign) len = sprintf(buf, "%d.%06d", integer, frac);
	else len = sprintf(buf, "-%d.%06d", integer, frac);

	return __stdio_fwrite(buf, len, stream);
}

static void modify_vfprintf() {
	/* TODO: Implement this function to hijack the formating of "%f"
	 * argument during the execution of `_vfprintf_internal'. Below
	 * is the code section in _vfprintf_internal() relative to the
	 * hijack.
	 */	

	/* mprotect((void *)((???) & 0xfffff000), 4096*2, PROT_READ | PROT_WRITE | PROT_EXEC);
	 * ???: 'call' instrcution address backward 100 bytes
	 */
	//mprotect((void *)(((unsigned int)&_vfprintf_internal + 0x306 - 100) & 0xfffff000), 4096*2, PROT_READ | PROT_WRITE | PROT_EXEC);

	/* the address of the 'call' instruction is (&_vfprintf_internal + 774)
	 * and one more byte for the first byte of rel instuction's dis
	 */
	uint32_t* locate = (uint32_t *)(&_vfprintf_internal + 0x307);

	/* modify the 'call' dis */
	int delta = (uint32_t)&_fpmaxtostr - (uint32_t)&format_FLOAT;
	*locate = (int)*locate - delta;

	/* modify the instruction that send arguments */
	/* initial : 8048xxx: db 3c 24	fstpt (%esp)
	 * modified: 8048xxx: ff 32	push (%edx)
	 * 	     8048xxx: 90	nop
	 */
	/* the address of the initial instruction */
	uint8_t *buf = (void *)(&_vfprintf_internal + 0x2fc);
	*buf = 0xff;
	*(buf + 1) = 0x32;
	*(buf + 2) = 0x90;

	/* modify the stack's depth */
	*(buf - 1) = 0x08;

	/* ignore other float instruction 'fldt', 'fldl' */
	buf = (void *)(&_vfprintf_internal + 0x2e4);
	*buf = 0x90;
	*(buf + 1) = 0x90;
	*(buf + 4) = 0x90;
	*(buf + 5) = 0x90;

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = _fpmaxtostr(stream,
				(__fpmax_t)
				(PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
				 ? *(long double *) *argptr
				 : (long double) (* (double *) *argptr)),
				&ppfs->info, FP_OUT );
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

	/* You should modify the run-time binary to let the code above
	 * call `format_FLOAT' defined in this source file, instead of
	 * `_fpmaxtostr'. When this function returns, the action of the
	 * code above should do the following:
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = format_FLOAT(stream, *(FLOAT *) *argptr);
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

}

static void modify_ppfs_setargs() {
	/* TODO: Implement this function to modify the action of preparing
	 * "%f" arguments for _vfprintf_internal() in _ppfs_setargs().
	 * Below is the code section in _vfprintf_internal() relative to
	 * the modification.
	 */
	
	/* get address before 'fldl' */
	/* initial : 8049xxx 8d 5a 08	lea 0x8(%edx),%ebx
	 * modified: 8049xxx eb 30	jmp 8049xxx
	 * 	     8049xxx 90		nop
	 */
	uint8_t *buf = (void *)(&_ppfs_setargs + 0x71);
	*buf = 0xeb;
	*(buf + 1) = 0x30;
	*(buf + 2) = 0x90;

#if 0
	enum {                          /* C type: */
		PA_INT,                       /* int */
		PA_CHAR,                      /* int, cast to char */
		PA_WCHAR,                     /* wide char */
		PA_STRING,                    /* const char *, a '\0'-terminated string */
		PA_WSTRING,                   /* const wchar_t *, wide character string */
		PA_POINTER,                   /* void * */
		PA_FLOAT,                     /* float */
		PA_DOUBLE,                    /* double */
		__PA_NOARG,                   /* non-glibc -- signals non-arg width or prec */
		PA_LAST
	};

	/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
	/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK		(0xff00)
#define __PA_FLAG_CHAR		(0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT		(0x0200)
#define PA_FLAG_LONG		(0x0400)
#define PA_FLAG_LONG_LONG	(0x0800)
#define PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define PA_FLAG_PTR		(0x1000) /* TODO -- make dynamic??? */

	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			case (PA_INT|PA_FLAG_LONG):
				GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
				break;
			case PA_CHAR:	/* TODO - be careful */
				/* ... users could use above and really want below!! */
			case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
			case (PA_INT|PA_FLAG_SHORT):
			case PA_INT:
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
				break;
			case PA_WCHAR:	/* TODO -- assume int? */
				/* we're assuming wchar_t is at least an int */
				GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
				break;
				/* PA_FLOAT */
			case PA_DOUBLE:
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
				GET_VA_ARG(p,ld,long double,ppfs->arg);
				break;
			default:
				/* TODO -- really need to ensure this can't happen */
				assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
			case PA_POINTER:
			case PA_STRING:
			case PA_WSTRING:
				GET_VA_ARG(p,p,void *,ppfs->arg);
				break;
			case __PA_NOARG:
				continue;
		}
		++p;
	}
#endif

	/* You should modify the run-time binary to let the `PA_DOUBLE'
	 * branch execute the code in the `(PA_INT|PA_FLAG_LONG_LONG)'
	 * branch. Comparing to the original `PA_DOUBLE' branch, the
	 * target branch will also prepare a 64-bit argument, without
	 * introducing floating point instructions. When this function
	 * returns, the action of the code above should do the following:
	 */

#if 0
	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
			here:
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			// ......
				/* PA_FLOAT */
			case PA_DOUBLE:
				goto here;
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			// ......
		}
		++p;
	}
#endif

}

void init_FLOAT_vfprintf() {
	modify_vfprintf();
	modify_ppfs_setargs();
}