#include "fs/path.h"
#include "test.h"

#include <stdlib.h>
#include <string.h>

// define dependency on kzalloc
void *
kzalloc(size_t size)
{
    return malloc(size);
}

// define dependency on kfree
void
kfree(void *ptr)
{
    free(ptr);
}

int
test_path_to_file(void *state)
{
    char file_path[] = "hdd0:/home/test_file.txt";

    struct path *path;
    path_from_str(&path, file_path);

    assert_true(strncmp(path->disk_id, "hdd0", 4) == 0);
    assert_true(strncmp(path->root->element, "home", 4) == 0);
    assert_true(strncmp(path->root->next->element, "test_file.txt", 13) == 0);
    assert_true(path->root->next->next == 0);

    return 0;
}

int
test_path_to_directory(void *state)
{
    char file_path[] = "hdd0:/home/test_dir";

    struct path *path;
    path_from_str(&path, file_path);

    assert_true(strncmp(path->disk_id, "hdd0", 4) == 0);
    assert_true(strncmp(path->root->element, "home", 4) == 0);
    assert_true(strncmp(path->root->next->element, "test_dir", 8) == 0);
    assert_true(path->root->next->next == 0);

    return 0;
}

int
test_path_to_directory_trailing_slash(void *state)
{
    char file_path[] = "hdd0:/home/test_dir/";

    struct path *path;
    path_from_str(&path, file_path);

    assert_true(strncmp(path->disk_id, "hdd0", 4) == 0);
    assert_true(strncmp(path->root->element, "home", 4) == 0);
    assert_true(strncmp(path->root->next->element, "test_dir", 8) == 0);
    assert_true(path->root->next->next == 0);

    return 0;
}

int
test_disk_id_too_long(void *state)
{
    char file_path[] = "verylongid:/home/test_file.txt";

    struct path *path;
    int res = path_from_str(&path, file_path);

    assert_true(res < 0);

    return 0;
}

int
test_illegal_disk_separator(void *state)
{
    char file_path[] = "hdd0::home/test_file.txt";

    struct path *path;
    int res = path_from_str(&path, file_path);

    assert_true(res < 0);

    return 0;
}

struct test_case test_cases[] = {
    {.name = "path_from_str - path to file",
     .test_function = test_path_to_file,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "path_from_str - path to directory",
     .test_function = test_path_to_directory,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "path_from_str - path to directory with trailing slash",
     .test_function = test_path_to_directory_trailing_slash,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "path_from_str - disk id too long",
     .test_function = test_disk_id_too_long,
     .setup_fixture = 0,
     .teardown_fixture = 0},
    {.name = "path_from_str - illegal disk id separator",
     .test_function = test_illegal_disk_separator,
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