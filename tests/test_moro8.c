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
    moro8_connect_memory(&vm, (moro8_bus*)&memory);
    assert_ptr_equal(vm.memory, &memory.bus);

    moro8_connect_memory(&vm, NULL);
    assert_null(vm.memory);
}

/** Test creating a backup and restoring it */
static void test_copy(void** state) {
    moro8_vm* vm = *state;

    moro8_set_progmem_word(vm, 0, 0xFF);

    // Backup state
    moro8_vm* snapshot = moro8_create();
    moro8_array_memory* memory = moro8_array_memory_create();
    moro8_connect_memory(snapshot, memory);
    assert_false(moro8_equal(vm, snapshot));
    moro8_copy(snapshot, vm);
    assert_true(moro8_equal(vm, snapshot));

    // Reset state
    moro8_set_progmem_word(vm, 0, 0);
    assert_false(moro8_equal(vm, snapshot));

    // Restore state
    moro8_copy(vm, snapshot);
    assert_true(moro8_equal(vm, snapshot));
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

/** Test moro8_set_register on a single register */
static void _test_set_register(moro8_register reg)
{
    moro8_vm* vm = moro8_create();

#define REGISTER_FUNC(reg) moro8_assert_register_equal(vm, reg, 0)
    // All registers must be 0
    MORO8_ALL_REGISTERS(REGISTER_FUNC);

    // Assign the reg register
#define REGISTER_VALUE 0x1    
    moro8_set_register(vm, reg, REGISTER_VALUE);

    // Other registers must be 0
    MORO8_OTHER_REGISTERS(REGISTER_FUNC, reg);

    // The reg register must have the right value
    moro8_assert_register_equal(vm, reg, REGISTER_VALUE);
#undef REGISTER_VALUE
#undef REGISTER_FUNC

    moro8_delete(vm);
}

/** Test moro8_set_register on each registers */
static void test_set_register(void** state) {
    moro8_vm* vm = *state;

    MORO8_ALL_REGISTERS(_test_set_register);
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

    // Print vm1 state and write to file
    char* dump = moro8_print(vm1);
    assert_true(fs_write_file(buf, dump, MORO8_PRINT_BUFFER_SIZE));
    free(dump);

    // Read file and parse vm state
    moro8_vm* vm2 = moro8_create();

    size_t size = 0;
    dump = fs_assert_read_file(buf, &size);
    moro8_parse(vm2, dump, size);
    free(dump);

    // Print vm2 state and write to file
    dump = moro8_print(vm2);
    fs_assert_join_path(&buf, output_dir, "test_print2.txt");
    assert_true(fs_write_file(buf, dump, MORO8_PRINT_BUFFER_SIZE));
    free(dump);

    // Check both vm are equal
    assert_true(moro8_equal(vm1, vm2));

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
        cmocka_unit_test_setup_teardown(test_set_register, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_word, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_dword, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_dword_oom, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_parse, moro8_setup_vm, moro8_delete_vm)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}