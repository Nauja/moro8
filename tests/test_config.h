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
typedef struct moro8_cpu moro8_cpu;

static struct moro8_cpu* _moro8_create()
{
	struct moro8_cpu* cpu = moro8_create();
	assert_non_null(cpu);
	return cpu;
}

static struct moro8_cpu* _moro8_parse(struct moro8_cpu* cpu, const char* buf, size_t size)
{
	assert_non_null(moro8_parse(cpu, buf, size));
	return cpu;
}

static void _moro8_equal(struct moro8_cpu* left, struct moro8_cpu* right)
{
	assert_true(moro8_equal(left, right));
}

static void _moro8_assert_memory_equal(struct moro8_cpu* cpu, const moro8_uword* buf, moro8_udword offset, moro8_udword size)
{
	moro8_uword value = 0;
	for (moro8_udword i = 0; i < size; ++i)
	{
		moro8_get_memory_word(cpu, offset + i);
		assert_int_equal(value, buf[i]);
	}
}

#define moro8_assert_create() _moro8_create()
#define moro8_assert_parse(cpu, buf, size) _moro8_parse(cpu, buf, size)
#define moro8_assert_equal(left, right) _moro8_equal(left, right)
#define moro8_assert_register_equal(cpu, reg, value) assert_int_equal(moro8_get_register(cpu, reg), value)
#define moro8_assert_register_not_equal(cpu, reg, value) assert_int_not_equal(moro8_get_register(cpu, reg), value)
#define moro8_assert_pc_equal(cpu, value) assert_int_equal(moro8_get_pc(cpu), value)
#define moro8_assert_accumulator_equal(cpu, value) assert_int_equal(moro8_get_register(cpu, MORO8_REGISTER_A), value)
#define moro8_assert_x_equal(cpu, value) assert_int_equal(moro8_get_x(cpu), value)
#define moro8_assert_y_equal(cpu, value) assert_int_equal(moro8_get_y(cpu), value)
#define moro8_assert_memory_equal(cpu, buf, offset, size) _moro8_assert_memory_equal(cpu, buf, offset, size)
#define moro8_assert_progmem_equal(cpu, buf, offset, size) _moro8_assert_memory_equal(cpu, buf, MORO8_PROGMEM_OFFSET + offset, size)
#define moro8_assert_zp_word_equal(cpu, offset, value) { moro8_uword __value = 0; moro8_get_zp_word(cpu, offset, __value); assert_int_equal(__value, value); }
#define moro8_assert_ram_word_equal(cpu, offset, value) { moro8_uword __value = 0; moro8_get_ram_word(cpu, offset, __value); assert_int_equal(__value, value); }

static void moro8_assert_output_dir(char (*buf)[LIBFS_MAX_PATH])
{
	char cwd[LIBFS_MAX_PATH];
	fs_assert_current_dir(&cwd);

	fs_assert_join_path(buf, cwd, DIRECTORY_OUTPUT);

	assert_true(fs_make_dir(buf[0]));
}

/** Setup a new CPU before running a test */
static int moro8_setup_vm(void** state) {
	moro8_cpu* cpu = moro8_create();
	cpu->memory = (moro8_bus*)moro8_array_memory_create();
    *state = (void*)cpu;
    return 0;
}

/** Delete the cpu after running a test */
static int moro8_delete_vm(void** state) {
	moro8_cpu* cpu = (struct moro8_cpu*)*state;
	moro8_array_memory_delete((moro8_array_memory*)cpu->memory);
    moro8_delete(cpu);
    *state = NULL;
    return 0;
}