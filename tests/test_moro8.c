#include "test_config.h"

#define FILE_HELLO "data/hello.txt"
#define FILE_UNKNOWN "unknown"

/** Test types have the right size */
static void test_types(void **state)
{
    assert_int_equal(sizeof(moro8_uword), 1);
    assert_int_equal(sizeof(moro8_word), 1);
    assert_int_equal(sizeof(moro8_udword), 2);
    assert_int_equal(sizeof(moro8_dword), 2);
}

/** Test allocating a new cpu */
static void test_create(void **state)
{
    moro8_cpu *cpu = moro8_create();
    assert_non_null(cpu);
    assert_null(cpu->memory);

    moro8_delete(cpu);
}

/** Test initializing an already allocated cpu */
static void test_init(void **state)
{
    moro8_cpu cpu;
    moro8_init(&cpu);
    assert_null(cpu.memory);
}

/** Test connecting moro8_array_memory */
static void test_connect_memory(void **state)
{
    moro8_cpu cpu;
    moro8_init(&cpu);
    assert_null(cpu.memory);

    moro8_array_memory memory;
    moro8_set_memory_bus(&cpu, (moro8_bus *)&memory);
    assert_ptr_equal(cpu.memory, &memory.bus);

    moro8_set_memory_bus(&cpu, NULL);
    assert_null(cpu.memory);
}

/** Test creating a backup and restoring it */
static void test_copy(void **state)
{
    moro8_cpu *cpu = *state;

    moro8_set_memory_word(cpu, MORO8_ROM_OFFSET, 0xFF);

    // Backup state
    moro8_cpu *snapshot = moro8_create();
    moro8_array_memory *memory = moro8_array_memory_create();
    moro8_set_memory_bus(snapshot, (moro8_bus *)memory);
    assert_false(moro8_equal(cpu, snapshot));
    moro8_copy(snapshot, cpu);
    assert_true(moro8_equal(cpu, snapshot));

    // Reset state
    moro8_set_memory_word(cpu, MORO8_ROM_OFFSET, 0);
    assert_false(moro8_equal(cpu, snapshot));

    // Restore state
    moro8_copy(cpu, snapshot);
    assert_true(moro8_equal(cpu, snapshot));

    moro8_array_memory_delete(memory);
    moro8_delete(snapshot);
}

/** Test shown in README.md */
static void test_readme_addition(void **state)
{
    moro8_cpu *cpu = *state;

    moro8_uword prog[] = {
        0xA9, 0x02, // LDA #$02
        0x69, 0x03  // ADC #$03
    };
    moro8_load(cpu, prog, 4);
    moro8_run(cpu);

    printf("Result of 2 + 3 is %d", moro8_get_ac(cpu));
    assert_int_equal(moro8_get_ac(cpu), 5);
}

/** Test setting a single word to memory. */
static void test_set_memory_word(void **state)
{
    moro8_cpu *cpu = *state;

    assert_int_equal(moro8_get_memory_word(cpu, 0x1000), 0);

    moro8_set_memory_word(cpu, 0x1000, 0xFF);

    assert_int_equal(moro8_get_memory_word(cpu, 0x1000), 0xFF);
}

/** Test setting a double word to memory. */
static void test_set_memory_dword(void **state)
{
    moro8_cpu *cpu = *state;

    assert_int_equal(moro8_get_memory_dword(cpu, 0x1000), 0);

    moro8_set_memory_dword(cpu, 0x1000, 0xFF10);

    assert_int_equal(moro8_get_memory_word(cpu, 0x1000), 0x10);
    assert_int_equal(moro8_get_memory_word(cpu, 0x1001), 0xFF);

    assert_int_equal(moro8_get_memory_dword(cpu, 0x1000), 0xFF10);
}

/**
 * Test setting a double word at address MORO8_MEMORY_SIZE - 1.
 *
 * This should fill only the low byte.
 */
static void test_set_memory_dword_oom(void **state)
{
    moro8_cpu *cpu = *state;

    assert_int_equal(moro8_get_memory_dword(cpu, MORO8_MEMORY_SIZE - 1), 0);

    moro8_set_memory_dword(cpu, MORO8_MEMORY_SIZE - 1, 0xFF10);

    assert_int_equal(moro8_get_memory_word(cpu, MORO8_MEMORY_SIZE - 1), 0x10);

    assert_int_equal(moro8_get_memory_dword(cpu, MORO8_MEMORY_SIZE - 1), 0x10);
}

/** Test parsing words. */
static void test_parse_word(void **state)
{
    moro8_uword value;
    char buf[] = " A  2 B ";
    assert_int_equal(moro8_parse_word(buf, 10, &value), 5);
    assert_int_equal(value, 0xA2);
}

/** Test parsing words. */
static void test_parse_word_invalid(void **state)
{
    moro8_uword value;
    char buf[] = " A ";
    assert_int_equal(moro8_parse_word(buf, 4, &value), 0);
    assert_int_equal(value, 0xA0);
}

/** Test parsing double words. */
static void test_parse_dword(void **state)
{
    moro8_udword value;
    char buf[] = " A  2 B 5 D ";
    assert_int_equal(moro8_parse_dword(buf, 14, &value), 9);
    assert_int_equal(value, 0xA2B5);
}

/** Test parsing double words. */
static void test_parse_dword_invalid(void **state)
{
    moro8_udword value;
    char buf[] = " A  2 B ";
    assert_int_equal(moro8_parse_dword(buf, 10, &value), 0);
    assert_int_equal(value, 0xA200);
}

/** Test printing a cpu state and parsing it back. */
static void test_parse(void **state)
{
    moro8_cpu *vm1 = *state;

    char output_dir[LIBFS_MAX_PATH];
    moro8_assert_output_dir(&output_dir);

    char buf[LIBFS_MAX_PATH];
    fs_assert_join_path(&buf, output_dir, "test_print.txt");

    // Print vm1 state and write to test_print.txt
    char dump[MORO8_PRINT_BUFFER_SIZE];
    assert_int_equal(moro8_print(vm1, dump, MORO8_PRINT_BUFFER_SIZE), MORO8_PRINT_BUFFER_SIZE - 1);
    assert_true(dump[MORO8_PRINT_BUFFER_SIZE - 1] == '\0');
    assert_true(fs_write_file(buf, dump, MORO8_PRINT_BUFFER_SIZE - 1)); // MORO8_PRINT_BUFFER_SIZE count the null-terminating character

    // Create another cpu
    moro8_cpu *vm2 = moro8_create();
    vm2->memory = (moro8_bus *)moro8_array_memory_create();

    // Read file and parse cpu state
    size_t size = 0;
    void *content = fs_assert_read_file(buf, &size);
    assert_int_equal(size, MORO8_PRINT_BUFFER_SIZE - 1);
    moro8_parse(vm2, content, size);
    free(content);

    // Print vm2 state and write to test_print2.txt
    assert_int_equal(moro8_print(vm2, dump, MORO8_PRINT_BUFFER_SIZE), MORO8_PRINT_BUFFER_SIZE - 1);
    assert_true(dump[MORO8_PRINT_BUFFER_SIZE - 1] == '\0');
    fs_assert_join_path(&buf, output_dir, "test_print2.txt");
    assert_true(fs_write_file(buf, dump, MORO8_PRINT_BUFFER_SIZE - 1)); // MORO8_PRINT_BUFFER_SIZE count the null-terminating character

    // Check both cpu are equal
    assert_true(moro8_equal(vm1, vm2));

    moro8_array_memory_delete((moro8_array_memory *)vm2->memory);
    moro8_delete(vm2);
}

/** Test the special status register. */
static void test_sr(void **state)
{
    moro8_cpu *cpu = *state;

    assert_int_equal(moro8_get_sr(cpu), 0);

    assert_int_equal(moro8_get_n(cpu), 0);
    moro8_set_n(cpu, 1);
    assert_int_equal(moro8_get_n(cpu), 1);
    assert_int_equal(moro8_get_sr(cpu), 0x80);
    moro8_set_n(cpu, 0xFF);
    assert_int_equal(moro8_get_n(cpu), 1);
    assert_int_equal(moro8_get_sr(cpu), 0x80);
    moro8_set_n(cpu, 0);
    assert_int_equal(moro8_get_n(cpu), 0);
    assert_int_equal(moro8_get_sr(cpu), 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_types),
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_init),
        cmocka_unit_test(test_connect_memory),
        cmocka_unit_test(test_parse_word),
        cmocka_unit_test(test_parse_word_invalid),
        cmocka_unit_test(test_parse_dword),
        cmocka_unit_test(test_parse_dword_invalid),
        cmocka_unit_test_setup_teardown(test_copy, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_readme_addition, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_word, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_dword, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_set_memory_dword_oom, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_parse, moro8_setup_vm, moro8_delete_vm),
        cmocka_unit_test_setup_teardown(test_sr, moro8_setup_vm, moro8_delete_vm)};
    return cmocka_run_group_tests(tests, NULL, NULL);
}