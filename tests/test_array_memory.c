#include "test_config.h"

/** Test allocating moro8_array_memory */
static void test_create(void** state) {
    moro8_array_memory* memory = moro8_array_memory_create();
    assert_non_null(memory);
    assert_non_null(memory->bus.get);
    assert_non_null(memory->bus.set);
    assert_non_null(memory->bus.get_word);
    assert_non_null(memory->bus.set_word);
    assert_non_null(memory->bus.get_dword);
    assert_non_null(memory->bus.set_dword);
    assert_non_null(memory->bus.reset);

    moro8_array_memory_delete(memory);
}

/** Test allocating moro8_array_memory */
static void test_init(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);
    assert_non_null(memory.bus.get);
    assert_non_null(memory.bus.set);
    assert_non_null(memory.bus.get_word);
    assert_non_null(memory.bus.set_word);
    assert_non_null(memory.bus.get_dword);
    assert_non_null(memory.bus.set_dword);
    assert_non_null(memory.bus.reset);
}

/** Test set words */
static void test_set_word(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);

    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFF), 0);
    moro8_memory_set_word((moro8_bus*)&memory, 0xFF, 0xFF);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFF), 0xFF);
}

/** Test set dwords */
static void test_set_dword(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);

    assert_int_equal(moro8_memory_get_dword((moro8_bus*)&memory, 0xFE), 0);
    moro8_memory_set_dword((moro8_bus*)&memory, 0xFE, 0x8004);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFE), 0x04);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFF), 0x80);
    assert_int_equal(moro8_memory_get_dword((moro8_bus*)&memory, 0xFE), 0x8004);
}

/** Test set buffer */
static void test_set(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);

    moro8_uword buffer[] = {0x04, 0x80};
    moro8_memory_set((moro8_bus*)&memory, buffer, 0xFE, 2);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFE), 0x04);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFF), 0x80);
    assert_int_equal(moro8_memory_get_dword((moro8_bus*)&memory, 0xFE), 0x8004);

    buffer[0] = 0;
    buffer[1] = 0;
    moro8_memory_get((moro8_bus*)&memory, buffer, 0xFE, 2);
    assert_int_equal(buffer[0], 0x04);
    assert_int_equal(buffer[1], 0x80);
}

/** Test reset */
static void test_reset(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);

    moro8_memory_set_word((moro8_bus*)&memory, 0xFF, 0xFF);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFF), 0xFF);
    moro8_memory_reset((moro8_bus*)&memory);
    assert_int_equal(moro8_memory_get_word((moro8_bus*)&memory, 0xFF), 0x00);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_init),
        cmocka_unit_test(test_set_word),
        cmocka_unit_test(test_set_dword),
        cmocka_unit_test(test_set),
        cmocka_unit_test(test_reset),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}