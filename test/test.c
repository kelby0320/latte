#include "test.h"

#include <stdio.h>
#include <string.h>

struct test_case sentinal;

void
test_init()
{
    memset(&sentinal, 0, sizeof(struct test_case));
}

void
run_tests(const struct test_case *tests)
{
    const struct test_case *test = tests;
    
    for (test; memcmp(test, &sentinal, sizeof(struct test_case)) != 0; test++) {
		/* Call Setup Fixture */
		void *state = NULL;
		if (test->setup_fixture != NULL) {
			state = test->setup_fixture();
		}
		
		/* Run Test */
		int ret = test->test_function(state);
		printf("%s: %s\n", test->name, ret == 0 ? "PASSED" : "FAILED");

		/* Call Teardown Fixture */
		if (test->teardown_fixture != NULL) {
			test->teardown_fixture();
		}
    }
}
