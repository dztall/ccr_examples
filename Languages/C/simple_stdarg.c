#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support va_list yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

#include <stdarg.h>
#include <stdio.h>

void my_f(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	for (int i = 0; i < n; i++)
	{
		printf("%d\n", va_arg(ap, int));
	}
	va_end(ap);
}

int main()
{
	my_f(3, 1, 2, 3);
	return 0;
}
