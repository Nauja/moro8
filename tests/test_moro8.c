#include "test_config.h"

#define FILE_HELLO "data/hello.txt"
#define FILE_UNKNOWN "unknown"

/** Test types have the right size */
static void test_types(void** state) {
    assert_int_equal(sizeof(moro8_uword), 1);
    assert_int_equal(sizeof(moro8_word), 1);
    assert_int_equal(sizeof(moro8_udword), 2);
    assert_int_equal(sizeof(moro8_dword), 2);
}

/** Test allocating a new vm */
static void test_create(void** state) {
    moro8_vm* vm = moro8_create();
    assert_non_null(vm);
    assert_null(vm->memory);

    moro8_delete(vm);
}

/** Test initializing an already allocated vm */
static void test_init(void** state) {
    moro8_vm vm;
    moro8_init(&vm);
    assert_null(vm.memory);
}

/** Test connecting moro8_array_memory */
static void test_connect_memory(void** state) {
    moro8_vm vm;
    moro8_init(&vm);
    assert_null(vm.memory);

    moro8_array_memory memory;
    moro8_set_memory_bus(&vm, (moro8_bus*)&memory);
    assert_ptr_equal(vm.memory, &memory.bus);

    moro8_set_memory_bus(&vm, NULL);
    assert_null(vm.memory);
}

/** Test creating a backup and restoring it */
static void test_copy(void** state) {
    moro8_vm* vm = *state;

    moro8_set_memory_word(vm, MORO8_ROM_OFFSET, 0xFF);

    // Backup state
    moro8_vm* snapshot = moro8_create();
    moro8_array_memory* memory = moro8_array_memory_create();
    moro8_set_memory_bus(snapshot, (moro8_bus*)memory);
    assert_false(moro8_equal(vm, snapshot));
    moro8_copy(snapshot, vm);
    assert_true(moro8_equal(vm, snapshot));

    // Reset state
    moro8_set_memory_word(vm, MORO8_ROM_OFFSET, 0);
    assert_false(moro8_equal(vm, snapshot));

    // Restore state
    moro8_copy(vm, snapshot);
    assert_true(moro8_equal(vm, snapshot));

    moro8_array_memory_delete(memory);
    moro8_delete(snapshot);
}

/** Test shown in README.md */
static void test_readme_addition(void** state) {
    moro8_vm* vm = *state;

    moro8_uword prog[] = {
        0xA9, 0x02, // LDA #$02
        0x69, 0x03  // ADC #$03
    };
    moro8_load(vm, prog, 4);
    moro8_run(vm);

    printf("Result of 2 + 3 is %d", moro8_get_ac(vm));
    assert_int_equal(moro8_get_ac(vm), 5);
}

/** Test setting a single word to memory. */
static void test_set_memory_word(void** state) {
    moro8_vm* vm = *state;

    assert_int_equal(moro8_get_memory_word(vm, 0x1000), 0);

    moro8_set_memory_word(vm, 0x1000, 0xFF);

    assert_int_equal(moro8_get_memory_word(vm, 0x1000), 0xFF);
}

/** Test setting a double word to memory. */
static void test_set_memory_dword(void** state) {
    moro8_vm* vm = *state;

    assert_int_equal(moro8_get_memory_dword(vm, 0x1000), 0);

    moro8_set_memory_dword(vm, 0x1000, 0xFF10);
    
    assert_int_equal(moro8_get_memory_word(vm, 0x1000), 0x10);
    assert_int_equal(moro8_get_memory_word(vm, 0x1001), 0xFF);

    assert_int_equal(moro8_get_memory_dword(vm, 0x1000), 0xFF10);
}

/**
 * Test setting a double word at address MORO8_MEMORY_SIZE - 1.
 * 
 * This should fill only the low byte.
 */
static void test_set_memory_dword_oom(void** state) {
    moro8_vm* vm = *state;

    assert_int_equal(moro8_get_memory_dword(vm, MORO8_MEMORY_SIZE - 1), 0);

    moro8_set_memory_dword(vm, MORO8_MEMORY_SIZE - 1, 0xFF10);

    assert_int_equal(moro8_get_memory_word(vm, MORO8_MEMORY_SIZE - 1), 0x10);

    assert_int_equal(moro8_get_memory_dword(vm, MORO8_MEMORY_SIZE - 1), 0x10);
}

/** Test printing a vm state and parsing it back. */
static void test_parse(void** state) {
    moro8_vm* vm1 = *state;

    char output_dir[LIBFS_MAX_PATH];
    moro8_assert_output_dir(&output_dir);

    char buf[LIBFS_MAX_PATH];
    fs_assert_join_path(&buf, output_dir, "test_print.txt");

    // Print vm1 state and write to test_print.txt
    char dump[MORO8_PRINT_BUFFER_SIZE];
    assert_int_equal(moro8_print(vm1, dump, MORO8_PRINT_BUFFER_SIZE), MORO8_PRINT_BUFFER_SIZE - 1);
    assert_true(dump[MORO8_PRINT_BUFFER_SIZE - 1] == '\0');
    assert_true(fs_write_file(buf, dump, MORO8_PRINT_BUFFER_SIZE - 1)); // MORO8_PRINT_BUFFER_SIZE count the null-terminating character

    // Create another vm
    moro8_vm* vm2 = moro8_create();
    vm2->memory = (moro8_bus*)moro8_array_memory_create();

    // Read file and parse vm state
    size_t size = 0;
    void* content = fs_assert_read_file(buf, &size);
    assert_int_equal(size, MORO8_PRINT_BUFFER_SIZE - 1);
    moro8_parse(vm2, content, size);
    free(content);

    // Print vm2 state and write to test_print2.txt
    assert_int_equal(moro8_print(vm2, dump, MORO8_PRINT_BUFFER_SIZE), MORO8_PRINT_BUFFER_SIZE - 1);
    assert_true(dump[MORO8_PRINT_BUFFER_SIZE - 1] == '\0');
    fs_assert_join_path(&buf, output_dir, "test_print2.txt");
    assert_true(fs_write_file(buf, dump, MORO8_PRINT_BUFFER_SIZE - 1)); // MORO8_PRINT_BUFFER_SIZE count the null-terminating character

    // Check both vm are equal
    assert_true(moro8_equal(vm1, vm2));

    moro8_array_memory_delete((moro8_array_memory*)vm2->memory);
    moro8_delete(vm2);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_types),
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_init),
        cmocka_unit_test(test_connect_memory),
        cmocka_unit_test_setup_teardown(test_copy, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_readme_addition, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_word, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_dword, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_dword_oom, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_parse, moro8_setup_vm, moro8_delete_vm)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}