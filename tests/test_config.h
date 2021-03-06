#pragma once
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <cmocka.h>
#include "tests/fs_testutils.h"
#include "moro8.h"

#define DIRECTORY_DATA "data"
#define DIRECTORY_OUTPUT "output"

typedef enum moro8_opcode moro8_opcode;
typedef enum moro8_register moro8_register;
typedef struct moro8_bus moro8_bus;
typedef struct moro8_registers moro8_registers;
typedef struct moro8_array_memory moro8_array_memory;
typedef struct moro8_vm moro8_vm;

static struct moro8_vm* _moro8_create()
{
	struct moro8_vm* vm = moro8_create();
	assert_non_null(vm);
	return vm;
}

static struct moro8_vm* _moro8_parse(struct moro8_vm* vm, const char* buf, size_t size)
{
	assert_non_null(moro8_parse(vm, buf, size));
	return vm;
}

static void _moro8_equal(struct moro8_vm* left, struct moro8_vm* right)
{
	assert_true(moro8_equal(left, right));
}

static void _moro8_assert_memory_equal(struct moro8_vm* vm, const moro8_uword* buf, moro8_udword offset, moro8_udword size)
{
	moro8_uword value = 0;
	for (moro8_udword i = 0; i < size; ++i)
	{
		moro8_get_memory_word(vm, offset + i);
		assert_int_equal(value, buf[i]);
	}
}

#define moro8_assert_create() _moro8_create()
#define moro8_assert_parse(vm, buf, size) _moro8_parse(vm, buf, size)
#define moro8_assert_equal(left, right) _moro8_equal(left, right)
#define moro8_assert_register_equal(vm, reg, value) assert_int_equal(moro8_get_register(vm, reg), value)
#define moro8_assert_register_not_equal(vm, reg, value) assert_int_not_equal(moro8_get_register(vm, reg), value)
#define moro8_assert_pc_equal(vm, value) assert_int_equal(moro8_get_pc(vm), value)
#define moro8_assert_accumulator_equal(vm, value) assert_int_equal(moro8_get_register(vm, MORO8_REGISTER_A), value)
#define moro8_assert_x_equal(vm, value) assert_int_equal(moro8_get_x(vm), value)
#define moro8_assert_y_equal(vm, value) assert_int_equal(moro8_get_y(vm), value)
#define moro8_assert_memory_equal(vm, buf, offset, size) _moro8_assert_memory_equal(vm, buf, offset, size)
#define moro8_assert_progmem_equal(vm, buf, offset, size) _moro8_assert_memory_equal(vm, buf, MORO8_PROGMEM_OFFSET + offset, size)
#define moro8_assert_zp_word_equal(vm, offset, value) { moro8_uword __value = 0; moro8_get_zp_word(vm, offset, __value); assert_int_equal(__value, value); }
#define moro8_assert_ram_word_equal(vm, offset, value) { moro8_uword __value = 0; moro8_get_ram_word(vm, offset, __value); assert_int_equal(__value, value); }

static void moro8_assert_output_dir(char (*buf)[LIBFS_MAX_PATH])
{
	char cwd[LIBFS_MAX_PATH];
	fs_assert_current_dir(&cwd);

	fs_assert_join_path(buf, cwd, DIRECTORY_OUTPUT);

	assert_true(fs_make_dir(buf[0]));
}

/** Setup a new VM before running a test */
static int moro8_setup_vm(void** state) {
	moro8_vm* vm = moro8_create();
	vm->memory = (moro8_bus*)moro8_array_memory_create();
    *state = (void*)vm;
    return 0;
}

/** Delete the vm after running a test */
static int moro8_delete_vm(void** state) {
	moro8_vm* vm = (struct moro8_vm*)*state;
	moro8_array_memory_delete((moro8_array_memory*)vm->memory);
    moro8_delete(vm);
    *state = NULL;
    return 0;
}