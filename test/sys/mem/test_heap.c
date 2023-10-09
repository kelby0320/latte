#include "mem/heap.h"
#include "test.h"
#include "config.h"

#include <stdlib.h>

#define HEAP_SIZE (LATTE_HEAP_MIN_BLOCK_SIZE * (1 << LATTE_HEAP_MAX_ORDER) * LATTE_HEAP_LARGE_BLOCKS)

static struct heap heap;

void*
setup_fixture()
{
    void *heap_addr = malloc(HEAP_SIZE);
    heap_init(&heap, heap_addr);
    return &heap;
}

void
teardown_fixture()
{
    free(heap.saddr);
}

int
test_heap_malloc_16_bytes(void *state)
{
    struct heap *heap = state;

    /*
      No allocations have yet occured.
      We are asking for 16 bytes, which is less the min block size (4k).
      We expected to have a 4k chunk allocated at the heap start address.
    */
    void *expected_ptr = heap->saddr;
    
    void *ptr = heap_malloc(heap, 16);

    assert_ptr_equal(ptr, expected_ptr);

    return 0;
}

int
test_heap_malloc_min_block(void *state)
{
    struct heap *heap = state;

    /*
      No allocations have yet occured.
      We are asking for the min block size (4k). 
      We expected to have a 4k chunk allocated at the heap start address.
    */
    void *expected_ptr = heap->saddr;
    
    void *ptr = heap_malloc(heap, LATTE_HEAP_MIN_BLOCK_SIZE);

    assert_ptr_equal(ptr, expected_ptr);

    return 0;
}

int
test_heap_malloc_mixed_allocations(void *state)
{
    struct heap *heap = state;

    void *ptr_3k = heap_malloc(heap, (1024 * 3));       /* order 0 block (4k) */
    void *ptr_6k = heap_malloc(heap, (1024 * 6));       /* order 1 block (8k) */
    void *ptr_12k = heap_malloc(heap, (1024 * 12));     /* order 2 block (16k) */
    void *ptr_30k = heap_malloc(heap, (1024 * 30));     /* order 3 block (32k) */
    void *ptr_60k = heap_malloc(heap, (1024 * 60));     /* order 4 block (64k) */
    void *ptr_120k = heap_malloc(heap, (1024 * 120));   /* order 5 block (128k) */
    void *ptr_230k = heap_malloc(heap, (1024 * 230));   /* order 6 block (256k) */
    void *ptr_480k = heap_malloc(heap, (1024 * 480));   /* order 7 block (512k) */
    void *ptr_900k = heap_malloc(heap, (1024 * 900));    /* order 8 block (1024k) */
    void *ptr_1800k = heap_malloc(heap, (1024 * 1800)); /* order 9 block (2048k) */
    void *ptr_3000k = heap_malloc(heap, (1024 * 3000)); /* order 10 block (4096k) */

    /* Ensure we received valid addresses */
    assert_not_null(ptr_3k);
    assert_not_null(ptr_6k);
    assert_not_null(ptr_12k);
    assert_not_null(ptr_30k);
    assert_not_null(ptr_60k);
    assert_not_null(ptr_120k);
    assert_not_null(ptr_230k);
    assert_not_null(ptr_480k);
    assert_not_null(ptr_900k);
    assert_not_null(ptr_1800k);
    assert_not_null(ptr_3000k);

    /* Ensure allocations are large enough for the size requested */
    assert_true((ptr_6k - ptr_3k) > (1024 * 3));
    assert_true((ptr_12k - ptr_6k) > (1024 * 6));
    assert_true((ptr_30k - ptr_12k) > (1024 * 12));
    assert_true((ptr_60k - ptr_30k) > (1024 * 30));
    assert_true((ptr_120k - ptr_60k) > (1024 * 60));
    assert_true((ptr_230k - ptr_120k) > (1024 * 120));
    assert_true((ptr_480k - ptr_230k) > (1024 * 230));
    assert_true((ptr_900k - ptr_480k) > (1024 * 480));
    assert_true((ptr_1800k - ptr_900k) > (1024 * 900));
    assert_true((ptr_3000k - ptr_1800k) > (1024 * 1800));

    return 0;
}

struct test_case test_cases[] = {
    {
        .name = "heap_malloc_16_bytes",
        .test_function = test_heap_malloc_16_bytes,
        .setup_fixture = setup_fixture,
        .teardown_fixture = teardown_fixture
    },
    {
        .name = "heap_malloc_min_block",
        .test_function = test_heap_malloc_min_block,
        .setup_fixture = setup_fixture,
        .teardown_fixture = teardown_fixture
    },
    {
        .name = "heap_malloc_mixed_allocations",
        .test_function = test_heap_malloc_mixed_allocations,
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
