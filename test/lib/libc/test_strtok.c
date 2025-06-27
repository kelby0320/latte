#include "string.h"
#include "test.h"

int
test_strtok_basic(void *state)
{
    char input[] = "foo bar baz";
    char *tok = strtok(input, " ");
    assert_string_equal(tok, "foo");
    tok = strtok(NULL, " ");
    assert_string_equal(tok, "bar");
    tok = strtok(NULL, " ");
    assert_string_equal(tok, "baz");
    tok = strtok(NULL, " ");
    assert_null(tok);
    return 0;
}

struct test_case test_cases[] = {
    {.name = "strtok basic", .test_function = test_strtok_basic, .setup_fixture = 0, .teardown_fixture = 0},
    {.name = 0, .test_function = 0, .setup_fixture = 0, .teardown_fixture = 0}};

int
main()
{
    test_init();
    run_tests(test_cases);
    return 0;
}
