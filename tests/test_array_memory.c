#include "test_config.h"

/** Test allocating moro8_array_memory */
static void test_create(void** state) {
    moro8_array_memory* memory = moro8_array_memory_create();
    assert_non_null(memory);
    assert_non_null(memory->bus.get);
    assert_non_null(memory->bus.set);

    moro8_array_memory_delete(memory);
}

/** Test allocating moro8_array_memory */
static void test_init(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);
    assert_non_null(memory.bus.get);
    assert_non_null(memory.bus.set);
}

/** Test that the bus is correctly initialized and can get/set the memory */
static void test_bus(void** state) {
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);

    memory.bus.set_word((moro8_bus*)&memory, 0xFF, 0);
    assert_int_equal(memory.bus.get_word((moro8_bus*)&memory, 0xFF), 0);
    memory.bus.set_word((moro8_bus*)&memory, 0xFF, 0xFF);
    assert_int_equal(memory.bus.get_word((moro8_bus*)&memory, 0xFF), 0xFF);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create),
        cmocka_unit_test(test_init),
        cmocka_unit_test(test_bus),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}