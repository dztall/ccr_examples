//Original source code found at : http://dev.ariel-networks.com/apr/apr-tutorial/sample/array-sample.c

/**
* apr tutorial sample code
* http://dev.ariel-networks.com/apr/
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <apr_general.h>
#include <apr_tables.h>
#include <apr_strings.h>

/* apr_array's initial size.
* because apr_array is a dynamic array, the initial size is not a max size of the array. */
#define ARRAY_INIT_SZ           32

static void append_items_to_array(apr_array_header_t *arr, apr_pool_t *mp)
{
	*(const char**)apr_array_push(arr) = "123";
	*(const char**)apr_array_push(arr) = "456";
	*(const char**)apr_array_push(arr) = "789";
	/* it's often a good idea to have elements in the same memory pool */
	*(const char**)apr_array_push(arr) = apr_pstrdup(mp, "abcdef");
	*(const char**)apr_array_push(arr) = apr_pstrdup(mp, "ghijkl");
}

static void iterate_array(const apr_array_header_t *arr)
{
	int i;
	for (i = 0; i < arr->nelts; i++) {
		const char *s = ((const char**)arr->elts)[i];
		printf("%d: %s\n", i, s);
	}
}

/**
* you can use array as stack (first-in, last-out)
*/
static void iterate_stack_array(apr_array_header_t *arr)
{
	const char **ppt;
	while ((ppt = apr_array_pop(arr))) {
		printf("pop'd string: %s\n", *ppt);
	}
}

/**
* apr_array sample code
* @remark Error checks omitted
*/
int main(int argc, const char *argv[])
{
	apr_pool_t *mp;
	apr_array_header_t *arr;

	apr_initialize();
	apr_pool_create(&mp, NULL);

	/* Create a dynamic array of string('const char*') */
	arr = apr_array_make(mp, ARRAY_INIT_SZ, sizeof(const char*));

	append_items_to_array(arr, mp);

	/* array iteration example */
	iterate_array(arr);

	/* concatenate the all items.
	* We can do this, because the array is the array of string. */
	printf("array concat = %s\n", apr_array_pstrcat(mp, arr, '\0'));

	/* yet another iteration, but removing the items */
	iterate_stack_array(arr);

	/* the array is destroyed when @mp is destroyed */
	apr_pool_destroy(mp);

	apr_terminate();
	return 0;
}
