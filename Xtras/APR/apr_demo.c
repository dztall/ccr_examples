//Original source code found at : http://people.apache.org/~rooneg/talks/portable-c-with-apr/apr.html

#include <apr.h>
#include <apr_pools.h>
#include <apr_file_io.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	apr_pool_t *pool;
	apr_file_t *out;

	apr_initialize();
	atexit(apr_terminate);

	apr_pool_create(&pool, NULL);
	apr_file_open_stdout(&out, pool);
	apr_file_printf(out, "Hello World\n");
	apr_pool_destroy(pool);

	return EXIT_SUCCESS;
}

