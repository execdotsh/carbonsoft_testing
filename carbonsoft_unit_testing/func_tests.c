#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "func.h"

void func_0_1(void **state)
{
	assert_int_equal(func(0, 1), -1);
}

void func_1_0(void **state)
{
	assert_int_equal(func(1, 0), -2);
}

void func_34_658590239(void **state)
{
	assert_int_equal(func(34, 658590239), 258641373);
}

int main() {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(func_0_1),
		cmocka_unit_test(func_1_0),
		cmocka_unit_test(func_34_658590239),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
