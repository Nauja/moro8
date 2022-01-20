// Here we only test opcodes
#include <tests/fs_testutils.h>
#include "test_config.h"

#define MORO8_TEST_OPCODES_DIR "test_opcodes"
#define MORO8_TEST_OPCODES_INPUT_DIR "test_opcodes"
#define MORO8_TEST_OPCODES_EXPECTED_DIR "test_opcodes/expected"
#define MORO8_TEST_OPCODES_OUTPUT_DIR "test_opcodes/output"
#define MORO8_NUM_TESTS 150

typedef struct fs_directory_iterator fs_directory_iterator;
typedef struct CMUnitTest CMUnitTest;

typedef struct test_state
{
    char input[LIBFS_MAX_PATH];
    char expected[LIBFS_MAX_PATH];
    char output[LIBFS_MAX_PATH];
    moro8_vm* vm;
} test_state;

/**
 * Run a test from test_opcodes directory.
 * @param[in] state Initial state
 */
void test_func(void** initial_state) {
    const test_state* state = (const test_state*)*initial_state;
    moro8_vm* vm = state->vm;

    // Run
    moro8_step(vm);

    // Print output state
    char output[MORO8_PRINT_BUFFER_SIZE];
    moro8_print(vm, output, MORO8_PRINT_BUFFER_SIZE);
    fs_assert_write_file(state->output, output, MORO8_PRINT_BUFFER_SIZE - 1);

    // Compare expected state
    size_t size = 0;
    const char* expected = (const char*)fs_assert_read_file(state->expected, &size);
    moro8_vm* vm2 = moro8_assert_create();
    vm2->memory = (moro8_bus*)moro8_array_memory_create();
    moro8_assert_parse(vm2, expected, size);
    moro8_assert_equal(vm, vm2);
    moro8_array_memory_delete((moro8_array_memory*)vm2->memory);
    moro8_delete(vm2);
}

int setup(void** initial_state)
{
    test_state* state = (test_state*)*initial_state;

    // Read test
    size_t size = 0;
    void* buf = NULL;
    buf = fs_assert_read_file(state->input, &size);

    // Parse test
    state->vm = moro8_assert_create();
    state->vm->memory = (moro8_bus*)moro8_array_memory_create();
    assert_true(moro8_parse(state->vm, buf, size));

    return 0;
}

int teardown(void** initial_state)
{
    test_state* state = (test_state*)*initial_state;

    // Delete vm
    moro8_array_memory_delete((moro8_array_memory*)state->vm->memory);
    moro8_delete(state->vm);
    free(state);

    return 0;
}

int main(void) {
    CMUnitTest tests[MORO8_NUM_TESTS];
    memset(tests, 0, sizeof(CMUnitTest) * MORO8_NUM_TESTS);

    // Current working directory
    char cwd[LIBFS_MAX_PATH];
    fs_assert_current_dir(&cwd);

    // Get test_opcodes directory
    char input_dir[LIBFS_MAX_PATH];
    char expected_dir[LIBFS_MAX_PATH];
    char output_dir[LIBFS_MAX_PATH];
    fs_assert_join_path(&input_dir, cwd, MORO8_TEST_OPCODES_INPUT_DIR);
    fs_assert_join_path(&expected_dir, cwd, MORO8_TEST_OPCODES_EXPECTED_DIR);
    fs_assert_join_path(&output_dir, cwd, MORO8_TEST_OPCODES_OUTPUT_DIR);
    fs_assert_make_dir(output_dir);

    // Iterate directory
    fs_directory_iterator* it = fs_assert_open_dir(input_dir);

    struct CMUnitTest* test = NULL;
    size_t test_index = 0;

    // Load all tests
    while(fs_read_dir(it))
    {
        if (!fs_string_ends_with(it->path, ".txt"))
        {
            continue;
        }

        // Setup initial state
        test_state* state = (test_state*)malloc(sizeof(test_state));
        fs_assert_join_path(&state->input, input_dir, it->path);
        fs_assert_join_path(&state->expected, expected_dir, it->path);
        fs_assert_join_path(&state->output, output_dir, it->path);

        // Setup test
        test = &tests[test_index];
        test->name = (char*)malloc(strlen(it->path));
        strcpy((char*)test->name, it->path);
        test->test_func = test_func;
        test->initial_state = (void*)state;
        test->setup_func = setup;
        test->teardown_func = teardown;

        ++test_index;
    }

    fs_close_dir(it);

    return cmocka_run_group_tests(tests, NULL, NULL);
}

#undef MORO8_NUM_TESTS