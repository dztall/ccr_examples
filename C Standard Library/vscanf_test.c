#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support va_list yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

#include <stdarg.h>

int vscanf(const char * format, va_list arg);
int printf(const char *fmt, ...);

void my_vscanf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vscanf(fmt, ap);
	va_end(ap);
}

int main()
{
	printf("input number : ");

	float f = 0.0f;
	my_vscanf("%f", &f);

	printf("%f", f);

	return 1;
}
