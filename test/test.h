#ifndef TEST_H
#define TEST_H

#include <string.h>

typedef void* (*SETUP_FIXTURE)(void);
typedef void (*TEARDOWN_FIXTURE)(void);
typedef int (*TEST_FUNCTION)(void *state);

struct test_case {
    char name[128];
    TEST_FUNCTION test_function;
    SETUP_FIXTURE setup_fixture;
    TEARDOWN_FIXTURE teardown_fixture;
};

#define assert_true(expr)			        \
    if (!(expr)) {				            \
	    return -1;				            \
    }

#define assert_false(expr)			        \
    if (expr) {					            \
	    return -1;				            \
    }

#define assert_null(ptr)			        \
    if ((void*)ptr != 0) {			        \
	    return -1;				            \
    }

#define assert_not_null(ptr)			    \
    if ((void*)ptr == 0) {			        \
	    return -1;				            \
    }

#define assert_ptr_equal(ptr1, ptr2)		\
    if ((void*)ptr1 != (void*)ptr2) {		\
	    return -1;				            \
    }

#define assert_ptr_not_equal(ptr1, ptr2)	\
    if ((void*)ptr1 == (void*)ptr2) {		\
	    return -1;				            \
    }

#define assert_char_equal(ch1, ch2)		    \
    if ((char)ch1 != (char)ch2) {		    \
	    return -1;				            \
    }

#define assert_char_not_equal(ch1, ch2)		\
    if ((char)ch1 == (char)ch2) {		    \
	    return -1;				            \
    }

#define assert_int_equal(i1, i2)		    \
    if ((int)i1 != (int)i2) {			    \
	    return -1;				            \
    }

#define assert_int_not_equal(i1, i2)		\
    if ((int)i1 == (int)i2) {			    \
	    return -1;				            \
    }

#define assert_string_equal(str1, str2)		\
    if (strcmp(str1, str2) != 0) {		    \
	    return -1;				            \
    }

#define assert_string_not_equal(str1, str2)	\
    if (strcmp(str1, str2) == 0) {		    \
	    return -1;				            \
    }

void
test_init();

void
run_tests(const struct test_case *tests);

#endif
