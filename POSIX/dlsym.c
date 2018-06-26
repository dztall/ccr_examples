#if defined(_OS_WINDOWS_)
#error "Not supported platform!!"
#endif

#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support script to native callback function yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

#include <stdio.h>
#include <dlfcn.h>

//Prototypes.
//void *dlopen(const char *filename,int flag);
//int dlclose(void *handle);
//void *dlsym(void *handle,const char *name);

typedef int(*sprintf_type)(char *, const char *, ...);
#ifdef __APPLE__
#define LIBC_PATH 0
#else
#ifdef __aarch64__
#define LIBC_PATH "/system/lib64/libc.so"
#else
#define LIBC_PATH "/system/lib/libc.so"
#endif
#endif

int main()
{
	void *libCModule = dlopen(LIBC_PATH, 0);
	if (libCModule)
	{
		sprintf_type proc = dlsym(libCModule, "sprintf");
		char buf[100];
		proc(buf, "%s %s!!", "hello", "world");
		dlclose(libCModule);
		printf("%s", buf);
	}

	return 0;
}
