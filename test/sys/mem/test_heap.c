#include "mem/heap.h"
#include "test.h"
#include "config.h"

#include <stdlib.h>

static void *heap_addr;

void*
setup_fixture()
{
    heap_addr = malloc(LATTE_HEAP_SIZE); /* 128 MB */
    return heap_addr;
}

void
teardown_fixture()
{
    free(heap_addr);
}

int
test_heap_init(void *state)
{
    struct heap heap;
    void *heap_addr = state;
    
    heap_init(&heap, heap_addr);

    return 0;
}

struct test_case test_cases[] = {
    {
	.name = "heap_init",
	.test_function = test_heap_init,
	.setup_fixture = setup_fixture,
	.teardown_fixture = teardown_fixture
    },
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
