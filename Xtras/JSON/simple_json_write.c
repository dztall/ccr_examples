#include <stdio.h>
#include <libjson.h>

int main()
{
	JSONNODE *n = json_new(JSON_NODE);
	json_push_back(n, json_new_a("String Node", "String Value"));
	json_push_back(n, json_new_i("Integer Node", 42));
	json_push_back(n, json_new_f("Floating Point Node", 3.14));
	json_push_back(n, json_new_b("Boolean Node", 1));
	json_char *jc = json_write_formatted(n);
	printf("%s\n", jc);
	json_free(jc);
	json_delete(n);

	return 1;
}