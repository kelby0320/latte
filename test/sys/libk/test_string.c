#include "libk/string.h"
#include "test.h"

int
test_strcat(void *state)
{
    char dest[16];
    memset(dest, 0, sizeof(dest));

    /* dest = "Hello"  */
    strcat(dest, "Hello");
    int ret = strcmp(dest, "Hello");
    assert_true(ret == 0);

    /* dest = "Hello World" */
    strcat(dest, " World");
    ret = strcmp(dest, "Hello World");
    assert_true(ret == 0);

    return 0;
}

int
test_strcmp_equal(void *state)
{
    char str1[] = "Hello World";
    char str2[] = "Hello World";

    int ret = strcmp(str1, str2);
    assert_true(ret == 0);

    return 0;
}

int
test_strcmp_not_equal(void *state)
{
    char str1[] = "Hello World";
    char str2[] = "Nether World";

    int ret = strcmp(str1, str2);
    assert_true(ret != 0);

    return 0;
}

int
test_strcpy(void *state)
{
    char src[] = "Hello World";
    char dest[16];

    strcpy(dest, src);
    int ret = strcmp(dest, "Hello World");
    assert_true(ret == 0);

    return 0;
}

int
test_strlen(void *state)
{
    char str[] = "Hello World";

    size_t len = strlen(str);
    assert_int_equal(len, 11);

    return 0;
}

int
test_strncat(void *state)
{
    char dest[16];
    memset(dest, 0, sizeof(dest));

    /* dest = "Hello"  */
    strncat(dest, "Hello", 6);
    int ret = strcmp(dest, "Hello");
    assert_true(ret == 0);

    /* dest = "Hello World" */
    strncat(dest, " World", 3);
    ret = strcmp(dest, "Hello Wo");
    assert_true(ret == 0);

    return 0;
}

int
test_strncmp_equal(void *state)
{
    char str1[] = "Hello World. How are you?";
    char str2[] = "Hello World. I am well.";

    int ret = strncmp(str1, str2, 11);
    assert_true(ret == 0);

    return 0;
}

int
test_strncmp_not_equal(void *state)
{
    char str1[] = "Hello World. How are you?";
    char str2[] = "Nether World. I am well.";

    int ret = strncmp(str1, str2, 11);
    assert_true(ret != 0);

    return 0;
}

int
test_strncpy_short_src(void *state)
{
    char src[] = "Hello World";
    char dest[16];

    strncpy(dest, src, 5);
    dest[5] = '\0'; // Add null terminator
    int ret = strcmp(dest, "Hello");
    assert_true(ret == 0);

    return 0;
}

int
test_strncpy_padding(void *state)
{
    char src[] = "Hello World";
    char dest[16];
    char zero[16];

    strncpy(dest, src, 16);
    int ret = strncmp(dest, "Hello World", 11);
    assert_true(ret == 0);

    memset(zero, 0, sizeof(zero));
    ret = memcmp(dest + 11, zero + 11, 5);
    assert_true(ret == 0);

    return 0;
}

struct test_case test_cases[] = {
    {.name = "strcat",
     .test_function = test_strcat,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strcmp - equal",
     .test_function = test_strcmp_equal,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strcmp - not equal",
     .test_function = test_strcmp_not_equal,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strcpy",
     .test_function = test_strcpy,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strlen",
     .test_function = test_strlen,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strncat",
     .test_function = test_strncat,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strncmp - equal",
     .test_function = test_strncmp_equal,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strncmp - not equal",
     .test_function = test_strncmp_not_equal,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strncpy - short src",
     .test_function = test_strncpy_short_src,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "strncpy - padding",
     .test_function = test_strncpy_padding,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    /* Sentinal Test Case  */
    {.name = 0, .test_function = 0, .setup_fixture = 0, .teardown_fixture = 0}};

int
main()
{
    test_init();
    run_tests(test_cases);
    return 0;
}
