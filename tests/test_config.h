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

#ifndef MAX_PATH
#define MAX_PATH 255
#endif

#define DIRECTORY_DATA "data"
#define DIRECTORY_OUTPUT "output"

typedef enum moro8_opcode moro8_opcode;
typedef enum moro8_register moro8_register;
typedef struct moro8_vm moro8_vm;

#define MORO8_ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))

#define MORO8_ALL_REGISTERS(fun) \
    fun(MORO8_REGISTER_PC); \
    fun(MORO8_REGISTER_A); \
    fun(MORO8_REGISTER_X); \
    fun(MORO8_REGISTER_Y); \
    fun(MORO8_REGISTER_S)

#define MORO8_OTHER_REGISTERS(fun, reg) \
    if (MORO8_REGISTER_PC != reg) fun(MORO8_REGISTER_PC); \
    if (MORO8_REGISTER_A != reg) fun(MORO8_REGISTER_A); \
    if (MORO8_REGISTER_X != reg) fun(MORO8_REGISTER_X); \
    if (MORO8_REGISTER_Y != reg) fun(MORO8_REGISTER_Y); \
    if (MORO8_REGISTER_S != reg) fun(MORO8_REGISTER_S)

#define MORO8_ALL_OPCODES(fun) \
	fun(MORO8_OP_LDA_IMM) \
	fun(MORO8_OP_LDA_ZP) \
	fun(MORO8_OP_LDA_ZP_X) \
	fun(MORO8_OP_LDA_ABS) \
	fun(MORO8_OP_LDA_ABS_X) \
	fun(MORO8_OP_LDA_ABS_Y) \
	fun(MORO8_OP_LDX_IMM) \
	fun(MORO8_OP_LDX_ZP) \
	fun(MORO8_OP_LDX_ZP_X) \
	fun(MORO8_OP_LDX_ABS) \
	fun(MORO8_OP_LDX_ABS_Y) \
	fun(MORO8_OP_LDY_IMM) \
	fun(MORO8_OP_LDY_ZP) \
	fun(MORO8_OP_LDY_ZP_X) \
	fun(MORO8_OP_LDY_ABS) \
	fun(MORO8_OP_LDY_ABS_X)

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

static void _moro8_assert_memory_equal(moro8_vm* vm, const moro8_uword* buf, moro8_udword offset, moro8_udword size)
{
	moro8_uword value = 0;
	for (moro8_udword i = 0; i < size; ++i)
	{
		moro8_get_memory_word(vm, offset + i, value);
		assert_int_equal(value, buf[i]);
	}
}

#define moro8_assert_create() _moro8_create()
#define moro8_assert_parse(vm, buf, size) _moro8_parse(vm, buf, size)
#define moro8_assert_equal(left, right) _moro8_equal(left, right)
#define moro8_assert_register_equal(vm, reg, value) assert_int_equal(moro8_get_register(vm, reg), value)
#define moro8_assert_register_not_equal(vm, reg, value) assert_int_not_equal(moro8_get_register(vm, reg), value)
#define moro8_assert_pc_equal(vm, value) assert_int_equal(moro8_get_register(vm, MORO8_REGISTER_PC), value)
#define moro8_assert_accumulator_equal(vm, value) assert_int_equal(moro8_get_register(vm, MORO8_REGISTER_A), value)
#define moro8_assert_x_equal(vm, value) assert_int_equal(moro8_get_register(vm, MORO8_REGISTER_X), value)
#define moro8_assert_y_equal(vm, value) assert_int_equal(moro8_get_register(vm, MORO8_REGISTER_Y), value)
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
    *state = (void*)moro8_create();
    return 0;
}

/** Delete the vm after running a test */
static int moro8_delete_vm(void** state) {
    moro8_delete((moro8_vm*)*state);
    *state = NULL;
    return 0;
}