#include "libk/memory.h"
#include "test.h"

int
test_memcmp_vals_equal(void *state)
{
    int vals1[] = {0, 1, 2, 3, 4, 5};
    int vals2[] = {0, 1, 2, 3, 4, 5};

    int ret = memcmp(&vals1, &vals2, sizeof(vals1));
    assert_true(ret == 0);

    return 0;
}

int
test_memcmp_vals_not_equal(void *state)
{
    int vals1[] = {0, 1, 2, 3, 4, 5};
    int vals2[] = {0, 5, 4, 3, 2, 1};

    int ret = memcmp(&vals1, &vals2, sizeof(vals1));
    assert_true(ret != 0);

    return 0;
}

int
test_memcpy(void *state)
{
    int src[] = {0, 1, 2, 3, 4, 5};
    int dest[sizeof(src)];

    /* Ensure dest != src */
    int ret = memcmp(&dest, &src, sizeof(dest));
    assert_true(ret != 0);

    /* Copy src to dest */
    memcpy(&dest, &src, sizeof(dest));

    /* Ensure dest == src */
    ret = memcmp(&dest, &src, sizeof(dest));
    assert_true(ret == 0);

    return 0;
}

int
test_memset(void *state)
{
    char buf[8];
    char expected[] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};

    memset(&buf, 'A', sizeof(buf));
    
    int ret = memcmp(&buf, &expected, sizeof(buf));
    assert_true(ret == 0);
    
    return 0;
}


struct test_case test_cases[] = {
    {
	.name = "memcmp - vals equal",
	.test_function = test_memcmp_vals_equal,
	.setup_fixture = 0,
	.teardown_fixture = 0
    },
    {
	.name = "memcmp - vals not equal",
	.test_function = test_memcmp_vals_not_equal,
	.setup_fixture = 0,
	.teardown_fixture = 0
    },
    {
	.name = "memcpy",
	.test_function = test_memcpy,
	.setup_fixture = 0,
	.teardown_fixture = 0
    },
    {
	.name = "memset",
	.test_function = test_memset,
	.setup_fixture = 0,
	.teardown_fixture = 0
    },
    /* Sentinal Test Case  */
    {
	.name = 0,
	.test_function = 0,
	.setup_fixture = 0,
	.teardown_fixture = 0
    }
};

int
main()
{
    test_init();
    run_tests(test_cases);
    return 0;
}
