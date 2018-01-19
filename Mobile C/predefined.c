#if !defined(__CCR__)
#error "Not supported platform!"
#endif

#include <stdio.h>

int main()
{
	printf("Mobile C App major version ( __CCR__ ) : %d\n", __CCR__);
	printf("Mobile C App minor version ( __CCR_MINOR__ ) : %d\n", __CCR_MINOR__);
	printf("Mobile C App patch level ( __CCR_PATCHLEVEL__ ) : %d\n\n", __CCR_PATCHLEVEL__);

#ifdef __CPP__
	printf("__CPP__ = %d\n", __CPP__);
	printf("__CPP_MINOR__ = %d\n", __CPP_MINOR__);
	printf("__CPP_PATCHLEVEL__ = %d\n\n", __CPP_PATCHLEVEL__);
#endif

#ifdef __GNUC__
	printf("__GNUC__ = %d\n", __GNUC__);
	printf("__GNUC_MINOR__ = %d\n", __GNUC_MINOR__);
	printf("__GNUC_PATCHLEVEL__ = %d\n\n", __GNUC_PATCHLEVEL__);
#endif

#ifdef __clang__
	printf("__clang__ = %d\n", __clang__);
	printf("__clang_major__ = %d\n", __clang_major__);
	printf("__clang_minor__ = %d\n", __clang_minor__);
	printf("__clang_patchlevel__ = %d\n", __clang_patchlevel__);
	printf("__clang_version__ = '%s'\n\n", __clang_version__);
#endif

#ifdef __TINYC__
	printf("__TINYC__ = %d\n\n", __TINYC__);
#endif

	printf("Documents directory : %s\n\n", __DOC_DIR__);
	printf("Source file being executed : %s\n\n", __FILE__);
	printf("Source file directory being executed : %s\n\n", __DIR__);

	return 0;
}
