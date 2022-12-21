#include "moro8.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if MORO8_WITH_PARSER
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

typedef struct moro8_hooks moro8_hooks;
typedef struct moro8_registers moro8_registers;
typedef struct moro8_bus moro8_bus;
typedef struct moro8_cpu moro8_cpu;
typedef enum moro8_register moro8_register;

#if defined(_MSC_VER)
/* work around MSVC error C2322: '...' address of dllimport '...' is not static */
static void *MORO8_CDECL internal_malloc(size_t size)
{
    return MORO8_MALLOC(size);
}

static void MORO8_CDECL internal_free(void *pointer)
{
    MORO8_FREE(pointer);
}
#else
#define internal_malloc MORO8_MALLOC
#define internal_free MORO8_FREE
#endif

static moro8_hooks moro8_global_hooks = {
    internal_malloc,
    internal_free};

MORO8_PUBLIC(void)
moro8_init_hooks(struct moro8_hooks *hooks)
{
    moro8_global_hooks.malloc_fn = hooks->malloc_fn;
    moro8_global_hooks.free_fn = hooks->free_fn;
}

#define _MORO8_MALLOC moro8_global_hooks.malloc_fn
#define _MORO8_FREE moro8_global_hooks.free_fn

#define MORO8_TRUE 1
#define MORO8_FALSE 0

#define _MORO8_MAX_ADDR (moro8_udword)(MORO8_MEMORY_SIZE - 1)
/** Gets the high part of a double word (16-bit) address. */
#define MORO8_HIGH(address) ((address & 0xFF00) >> 8)
/** Gets the low part of a double word (16-bit) address. */
#define MORO8_LOW(address) (address & 0xFF)
/** Easier access to registers. */
#define MORO8_PC cpu->registers.pc
#define MORO8_AC cpu->registers.ac
#define MORO8_X cpu->registers.x
#define MORO8_Y cpu->registers.y
#define MORO8_SR (*(moro8_uword *)&cpu->registers.sr)
#define MORO8_SP cpu->registers.sp
#define MORO8_C cpu->registers.sr.c
#define MORO8_Z cpu->registers.sr.z
#define MORO8_V cpu->registers.sr.v
#define MORO8_N cpu->registers.sr.n
/** Easier access to memory. */
#define MORO8_GET_MEM(addr) cpu->memory->get_word(cpu->memory, (moro8_udword)addr)
#define MORO8_SET_MEM(addr, value) cpu->memory->set_word(cpu->memory, (moro8_udword)addr, (moro8_uword)value)

#if !MORO8_MINIMALIST
typedef struct moro8_array_memory moro8_array_memory;

MORO8_PUBLIC(moro8_array_memory *)
moro8_array_memory_create()
{
    moro8_array_memory *memory = (moro8_array_memory *)_MORO8_MALLOC(sizeof(moro8_array_memory));
    moro8_array_memory_init(memory);
    return memory;
}

static moro8_udword moro8_array_memory_get(const struct moro8_bus *memory, moro8_uword *buffer, moro8_udword offset, moro8_udword size)
{
    moro8_udword max_size = MORO8_MEMORY_SIZE - offset;
    if (size > max_size)
    {
        size = max_size;
    }

    if (size <= 0)
    {
        return 0;
    }

    memcpy(buffer, &((moro8_array_memory *)memory)->buffer[offset], size);
    return size;
}

static moro8_udword moro8_array_memory_set(struct moro8_bus *memory, const moro8_uword *buffer, moro8_udword offset, moro8_udword size)
{
    moro8_udword max_size = MORO8_MEMORY_SIZE - offset;
    if (size > max_size)
    {
        size = max_size;
    }

    if (size <= 0)
    {
        return 0;
    }

    memcpy(&((moro8_array_memory *)memory)->buffer[offset], buffer, size);
    return size;
}

static moro8_uword moro8_array_memory_get_word(const struct moro8_bus *memory, moro8_udword address)
{
    return ((moro8_array_memory *)memory)->buffer[address];
}

static void moro8_array_memory_set_word(struct moro8_bus *memory, moro8_udword address, moro8_uword value)
{
    ((moro8_array_memory *)memory)->buffer[address] = value;
}

static moro8_udword moro8_array_memory_get_dword(const struct moro8_bus *memory, moro8_udword address)
{
    return (moro8_udword)((moro8_array_memory *)memory)->buffer[address] + ((address < _MORO8_MAX_ADDR) ? (((moro8_udword)((moro8_array_memory *)memory)->buffer[address + 1]) << 8) : 0);
}

static void moro8_array_memory_set_dword(struct moro8_bus *memory, moro8_udword address, moro8_udword value)
{
    ((moro8_array_memory *)memory)->buffer[address] = MORO8_LOW(value);
    if (address < _MORO8_MAX_ADDR)
    {
        ((moro8_array_memory *)memory)->buffer[address + 1] = MORO8_HIGH(value);
    }
}

static void moro8_array_memory_reset(struct moro8_bus *memory)
{
    memset(((moro8_array_memory *)memory)->buffer, 0, MORO8_MEMORY_SIZE);
}

MORO8_PUBLIC(void)
moro8_array_memory_init(moro8_array_memory *memory)
{
    memory->bus.get = &moro8_array_memory_get;
    memory->bus.set = &moro8_array_memory_set;
    memory->bus.get_word = &moro8_array_memory_get_word;
    memory->bus.set_word = &moro8_array_memory_set_word;
#if !MORO8_MINIMALIST
    memory->bus.get_dword = &moro8_array_memory_get_dword;
    memory->bus.set_dword = &moro8_array_memory_set_dword;
#endif
    memory->bus.reset = &moro8_array_memory_reset;

    memset(memory->buffer, 0, MORO8_MEMORY_SIZE);
}

MORO8_PUBLIC(void)
moro8_array_memory_delete(moro8_array_memory *memory)
{
    _MORO8_FREE(memory);
}
#endif

MORO8_PUBLIC(moro8_cpu *)
moro8_create()
{
    moro8_cpu *cpu = (moro8_cpu *)_MORO8_MALLOC(sizeof(moro8_cpu));
    moro8_init(cpu);
    return cpu;
}

static void _moro8_reset(moro8_cpu *cpu)
{
    // Reset registers to 0
    memset(&cpu->registers, 0, sizeof(moro8_registers));

    // Reset stack pointer to 0xFF
    cpu->registers.sp = MORO8_STACK_SIZE - 1;

    // Also reset memory to 0
    if (cpu->memory)
    {
        cpu->memory->reset(cpu->memory);
    }
}

MORO8_PUBLIC(void)
moro8_init(moro8_cpu *cpu)
{
    memset(cpu, 0, sizeof(moro8_cpu));

    _moro8_reset(cpu);
}

MORO8_PUBLIC(void)
moro8_delete(moro8_cpu *cpu)
{
    _MORO8_FREE(cpu);
}

MORO8_PUBLIC(const void *)
moro8_as_buffer(const moro8_cpu *cpu, size_t *size)
{
    return NULL;
}

MORO8_PUBLIC(void)
moro8_from_buffer(moro8_cpu *cpu, const void *buf, size_t size)
{
}

MORO8_PUBLIC(void)
moro8_load(moro8_cpu *cpu, const moro8_uword *prog, moro8_udword size)
{
    // Reset registers and memory
    _moro8_reset(cpu);

    // Load program to ROM
    moro8_set_memory(cpu, prog, MORO8_ROM_OFFSET, size);

    // Set program counter to ROM
    cpu->registers.pc = MORO8_ROM_OFFSET;
}

MORO8_PUBLIC(void)
moro8_resume(moro8_cpu *cpu)
{
    while (moro8_step(cpu))
    {
    }
}

/** Get a double word operand */
#define MORO8_DWORD_OPERAND (operand + (MORO8_GET_MEM(++cpu->registers.pc) << 8))
/** Get a double word at a memory address */
#define _MORO8_MEMORY_DWORD_CHECKED(addr) ((moro8_udword)MORO8_GET_MEM(addr) + ((addr < _MORO8_MAX_ADDR) ? (((moro8_udword)MORO8_GET_MEM(addr + 1)) << 8) : 0))
#define MORO8_MEMORY_DWORD(addr) _MORO8_MEMORY_DWORD_CHECKED(((moro8_udword)addr))
#define MORO8_MEMORY_SET_DWORD(addr, value)         \
    MORO8_SET_MEM(addr, MORO8_LOW(value));          \
    if (addr < _MORO8_MAX_ADDR)                     \
    {                                               \
        MORO8_SET_MEM(addr + 1, MORO8_HIGH(value)); \
    }
#define MORO8_DEC_PC --cpu->registers.pc
#define MORO8_TWO_COMPLEMENT(value) (~result + 1)
/** Build address depending on addressing mode. */
#define MORO8_ADDR_ZP operand
#define MORO8_ADDR_ZP_X (operand + MORO8_X)
#define MORO8_ADDR_ZP_Y (operand + MORO8_Y)
#define MORO8_ADDR_SP (MORO8_STACK_OFFSET + cpu->registers.sp + operand)
#define MORO8_ADDR_SP_X (MORO8_STACK_OFFSET + cpu->registers.sp + operand + MORO8_X)
#define MORO8_ADDR_SP_Y (MORO8_STACK_OFFSET + cpu->registers.sp + operand + MORO8_Y)
#define MORO8_ADDR_ABS MORO8_DWORD_OPERAND
#define MORO8_ADDR_ABS_X (MORO8_DWORD_OPERAND + MORO8_X)
#define MORO8_ADDR_ABS_Y (MORO8_DWORD_OPERAND + MORO8_Y)
#define MORO8_ADDR_IND_X (MORO8_MEMORY_DWORD(operand + MORO8_X))
#define MORO8_ADDR_IND_Y (MORO8_MEMORY_DWORD(operand) + MORO8_Y)
/** Get from memory depending on addressing mode. */
#define MORO8_GET_MEM_ZP() MORO8_GET_MEM(MORO8_ADDR_ZP)
#define MORO8_GET_MEM_ZP_X() MORO8_GET_MEM(MORO8_ADDR_ZP_X)
#define MORO8_GET_MEM_ZP_Y() MORO8_GET_MEM(MORO8_ADDR_ZP_Y)
#define MORO8_GET_MEM_SP() MORO8_GET_MEM(MORO8_ADDR_SP)
#define MORO8_GET_MEM_SP_X() MORO8_GET_MEM(MORO8_ADDR_SP_X)
#define MORO8_GET_MEM_SP_Y() MORO8_GET_MEM(MORO8_ADDR_SP_Y)
#define MORO8_GET_MEM_ABS() MORO8_GET_MEM(MORO8_ADDR_ABS)
#define MORO8_GET_MEM_ABS_X() MORO8_GET_MEM(MORO8_ADDR_ABS_X)
#define MORO8_GET_MEM_ABS_Y() MORO8_GET_MEM(MORO8_ADDR_ABS_Y)
#define MORO8_GET_MEM_IND_X() MORO8_GET_MEM(MORO8_ADDR_IND_X)
#define MORO8_GET_MEM_IND_Y() MORO8_GET_MEM(MORO8_ADDR_IND_Y)
/** Set to memory depending on addressing mode. */
#define MORO8_SET_MEM_ZP(value) MORO8_SET_MEM(MORO8_ADDR_ZP, value)
#define MORO8_SET_MEM_ZP_X(value) MORO8_SET_MEM(MORO8_ADDR_ZP_X, value)
#define MORO8_SET_MEM_ZP_Y(value) MORO8_SET_MEM(MORO8_ADDR_ZP_Y, value)
#define MORO8_SET_MEM_ABS(value) MORO8_SET_MEM(MORO8_ADDR_ABS, value)
#define MORO8_SET_MEM_ABS_X(value) MORO8_SET_MEM(MORO8_ADDR_ABS_X, value)
#define MORO8_SET_MEM_ABS_Y(value) MORO8_SET_MEM(MORO8_ADDR_ABS_Y, value)
#define MORO8_SET_MEM_IND_X(value) MORO8_SET_MEM(MORO8_ADDR_IND_X, value)
#define MORO8_SET_MEM_IND_Y(value) MORO8_SET_MEM(MORO8_ADDR_IND_Y, value)
/** */
#define MORO8_SIGN(value) (value & 0x80)
#define MORO8_IS_NEGATIVE(value) ((value & 0x80) != 0)
#define MORO8_SET_X(value)                \
    MORO8_X = value;                      \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_X); \
    MORO8_Z = MORO8_X == 0
#define MORO8_SET_Y(value)                \
    MORO8_Y = value;                      \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_Y); \
    MORO8_Z = MORO8_Y == 0
#define MORO8_SET_AC(value)                \
    MORO8_AC = value;                      \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_AC); \
    MORO8_Z = MORO8_AC == 0
#define MORO8_SET_SR(value) *((moro8_uword *)&cpu->registers.sr) = value
#define MORO8_SET_SP(value) MORO8_SP = value
#define MORO8_AND(operand) MORO8_SET_AC(MORO8_AC &operand)
#define MORO8_OR(operand) MORO8_SET_AC(MORO8_AC | operand)
#define MORO8_XOR(operand) MORO8_SET_AC(MORO8_AC ^ operand)

/**
 * Performs ACD opcode.
 * @param[in] cpu Some cpu
 * @param[in] operand Instruction operand
 */
static inline void _moro8_adc(moro8_cpu *cpu, moro8_uword operand)
{
    moro8_uword sign_bit = MORO8_SIGN(MORO8_AC) == MORO8_SIGN(operand);
    moro8_udword result = MORO8_AC + operand + MORO8_C;
    MORO8_C = result > 0xFF;
    MORO8_SET_AC((moro8_uword)(result & 0xFF));
    MORO8_V = sign_bit && (MORO8_SIGN(MORO8_AC) != MORO8_SIGN(operand));
}

/**
 * Performs BIT opcode.
 * @param[in] cpu Some cpu
 * @param[in] operand Instruction operand
 */
static inline void _moro8_bit(moro8_cpu *cpu, moro8_uword operand)
{
    MORO8_N = MORO8_IS_NEGATIVE(operand);
    MORO8_V = (operand & 0x70) != 0;
    MORO8_Z = (MORO8_AC & operand) == 0;
}

/**
 * Performs one of branch opcodes.
 * @param[in] condition The condition to check
 */
#define _moro8_branch(cpu, condition, operand) \
    {                                          \
        if (condition)                         \
        {                                      \
            MORO8_PC += ((moro8_word)operand); \
        }                                      \
    }

/**
 * Performs CMP/CPX/CPY opcode.
 * @param[in] cpu Some cpu
 * @param[in] left Value to compare
 * @param[in] right Value to compare
 */
static inline void _moro8_cmp(moro8_cpu *cpu, moro8_uword left, moro8_uword right)
{
    MORO8_N = left < right;
    MORO8_Z = left == right;
    MORO8_C = left >= right;
}

#define _moro8_cmp_ac(cpu, operand) _moro8_cmp(cpu, MORO8_AC, operand)
#define _moro8_cmp_x(cpu, operand) _moro8_cmp(cpu, MORO8_X, operand)
#define _moro8_cmp_y(cpu, operand) _moro8_cmp(cpu, MORO8_Y, operand)

/**
 * Performs DEC opcode.
 * @param[in] cpu Some cpu
 * @param[in] address Memory address
 */
static inline void _moro8_dec(moro8_cpu *cpu, moro8_udword address)
{
    moro8_uword value = MORO8_GET_MEM(address);
    value--;
    MORO8_N = MORO8_IS_NEGATIVE(value);
    MORO8_Z = value == 0;
    MORO8_SET_MEM(address, value);
}

/**
 * Performs DEX/Y opcode.
 * @param[in] cpu Some cpu
 * @param[in] reg Pointer to a register
 */
static inline void _moro8_dec_reg(moro8_cpu *cpu, moro8_uword *reg)
{
    (*reg)--;
    MORO8_N = MORO8_IS_NEGATIVE(*reg);
    MORO8_Z = (*reg) == 0;
}

/**
 * Performs INC opcode.
 * @param[in] cpu Some cpu
 * @param[in] address Memory address
 */
static inline void _moro8_inc(moro8_cpu *cpu, moro8_udword address)
{
    moro8_uword value = MORO8_GET_MEM(address);
    value++;
    MORO8_N = MORO8_IS_NEGATIVE(value);
    MORO8_Z = value == 0;
    MORO8_SET_MEM(address, value);
}

/**
 * Performs INX/Y opcode.
 * @param[in] cpu Some cpu
 * @param[in] reg Pointer to a register
 */
static inline void _moro8_inc_reg(moro8_cpu *cpu, moro8_uword *reg)
{
    (*reg)++;
    MORO8_N = MORO8_IS_NEGATIVE(*reg);
    MORO8_Z = (*reg) == 0;
}

/**
 * Performs ASL/ROL opcode with accumulator.
 * @param[in] cpu Some cpu
 * @param[in] lb Lower byte (0 for ASL, C for ROL)
 */
static inline void _moro8_rol_ac(moro8_cpu *cpu, moro8_uword lb)
{
    MORO8_C = MORO8_IS_NEGATIVE(MORO8_AC);
    MORO8_AC = ((MORO8_AC & 0x7F) << 1) + lb;
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_AC);
    MORO8_Z = MORO8_AC == 0;
}

/**
 * Performs ASL/ROL opcode with memory.
 * @param[in] cpu Some cpu
 * @param[in] address Memory address
 * @param[in] lb Lower byte (0 for ASL, C for ROL)
 */
static inline void _moro8_rol(moro8_cpu *cpu, moro8_udword address, moro8_uword lb)
{
    moro8_uword value = MORO8_GET_MEM(address);
    MORO8_C = MORO8_IS_NEGATIVE(value);
    value = ((value & 0x7F) << 1) + lb;
    MORO8_N = MORO8_IS_NEGATIVE(value);
    MORO8_Z = value == 0;
    MORO8_SET_MEM(address, value);
}

/**
 * Performs LSR/ROR opcode with accumulator.
 * @param[in] cpu Some cpu
 * @param[in] hb Higher byte (0 for LSR, C for ROR)
 */
static inline void _moro8_ror_ac(moro8_cpu *cpu, moro8_uword hb)
{
    MORO8_C = MORO8_AC & 0x1;
    MORO8_N = hb != 0;
    MORO8_AC = ((MORO8_AC & 0xFE) >> 1) + (hb << 7);
    MORO8_Z = MORO8_AC == 0;
}

/**
 * Performs LSR/ROR opcode with memory.
 * @param[in] cpu Some cpu
 * @param[in] address Memory address
 * @param[in] hb Higher byte (0 for LSR, C for ROR)
 */
static inline void _moro8_ror(moro8_cpu *cpu, moro8_udword address, moro8_uword hb)
{
    moro8_uword value = MORO8_GET_MEM(address);
    MORO8_C = value & 0x1;
    MORO8_N = hb != 0;
    value = ((value & 0xFE) >> 1) + (hb << 7);
    MORO8_Z = value == 0;
    MORO8_SET_MEM(address, value);
}

/**
 * Push a value on top of stack.
 * @param[in] cpu Some cpu
 * @param[in] value Some value
 * @return If it worked
 */
static inline int _moro8_push_stack(moro8_cpu *cpu, moro8_uword value)
{
    if (cpu->registers.sp == 0)
    {
        return MORO8_FALSE;
    }

    MORO8_SET_MEM(MORO8_STACK_OFFSET + cpu->registers.sp, value);
    cpu->registers.sp--;
    return MORO8_TRUE;
}

/**
 * Pop the value on top of stack.
 * @param[in] cpu Some cpu
 * @return Popped value
 */
static inline moro8_uword _moro8_pop_stack(moro8_cpu *cpu)
{
    if (cpu->registers.sp == 0xFF)
    {
        return 0;
    }

    cpu->registers.sp++;
    return MORO8_GET_MEM(MORO8_STACK_OFFSET + cpu->registers.sp);
}

/**
 * Performs SBC opcode.
 * @param[in] cpu Some cpu
 * @param[in] operand Instruction operand
 */
#define _moro8_sbc(cpu, operand)   \
    {                              \
        MORO8_C = 1;               \
        _moro8_adc(cpu, ~operand); \
    }

MORO8_PUBLIC(size_t)
moro8_step(moro8_cpu *cpu)
{
    moro8_uword instruction = MORO8_GET_MEM(cpu->registers.pc);

#if MORO8_WITH_HANDLERS
    // Lets custom handlers handle the instruction
    if (cpu->handlers[instruction] && cpu->handlers[instruction](cpu, instruction))
    {
        return MORO8_TRUE;
    }
#endif

    moro8_uword operand = MORO8_GET_MEM(++cpu->registers.pc);

    if (instruction == 0)
    {
        return MORO8_FALSE;
    }

    switch (instruction)
    {
    case MORO8_OP_ADC_IMM:
        _moro8_adc(cpu, operand);
        break;
    case MORO8_OP_ADC_ZP:
        _moro8_adc(cpu, MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_ADC_ZP_X:
        _moro8_adc(cpu, MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_ADC_ABS:
        _moro8_adc(cpu, MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_ADC_ABS_X:
        _moro8_adc(cpu, MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_ADC_ABS_Y:
        _moro8_adc(cpu, MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_ADC_IND_X:
        _moro8_adc(cpu, MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_ADC_IND_Y:
        _moro8_adc(cpu, MORO8_GET_MEM_IND_Y());
        break;
    case MORO8_OP_AND_IMM:
        MORO8_AND(operand);
        break;
    case MORO8_OP_AND_ZP:
        MORO8_AND(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_AND_ZP_X:
        MORO8_AND(MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_AND_ABS:
        MORO8_AND(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_AND_ABS_X:
        MORO8_AND(MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_AND_ABS_Y:
        MORO8_AND(MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_AND_IND_X:
        MORO8_AND(MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_AND_IND_Y:
        MORO8_AND(MORO8_GET_MEM_IND_Y());
        break;
    case MORO8_OP_ASL_AC:
        _moro8_rol_ac(cpu, 0);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_ASL_ZP:
        _moro8_rol(cpu, MORO8_ADDR_ZP, 0);
        break;
    case MORO8_OP_ASL_ZP_X:
        _moro8_rol(cpu, MORO8_ADDR_ZP_X, 0);
        break;
    case MORO8_OP_ASL_ABS:
        _moro8_rol(cpu, MORO8_ADDR_ABS, 0);
        break;
    case MORO8_OP_ASL_ABS_X:
        _moro8_rol(cpu, MORO8_ADDR_ABS_X, 0);
        break;
    case MORO8_OP_BCC:
        _moro8_branch(cpu, MORO8_C == 0, operand);
        break;
    case MORO8_OP_BCS:
        _moro8_branch(cpu, MORO8_C != 0, operand);
        break;
    case MORO8_OP_BEQ:
        _moro8_branch(cpu, MORO8_Z != 0, operand);
        break;
    case MORO8_OP_BIT_ZP:
        _moro8_bit(cpu, MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_BIT_ABS:
        _moro8_bit(cpu, MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_BMI:
        _moro8_branch(cpu, MORO8_N != 0, operand);
        break;
    case MORO8_OP_BNE:
        _moro8_branch(cpu, MORO8_Z == 0, operand);
        break;
    case MORO8_OP_BPL:
        _moro8_branch(cpu, MORO8_N == 0, operand);
        break;
    case MORO8_OP_BVC:
        _moro8_branch(cpu, MORO8_V == 0, operand);
        break;
    case MORO8_OP_BVS:
        _moro8_branch(cpu, MORO8_V != 0, operand);
        break;
    case MORO8_OP_CLC:
        MORO8_C = 0;
        MORO8_DEC_PC;
        break;
    case MORO8_OP_CLV:
        MORO8_V = 0;
        MORO8_DEC_PC;
        break;
    case MORO8_OP_CMP_IMM:
        _moro8_cmp_ac(cpu, operand);
        break;
    case MORO8_OP_CMP_ZP:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_CMP_ZP_X:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_CMP_ABS:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_CMP_ABS_X:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_CMP_ABS_Y:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_CMP_IND_X:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_CMP_IND_Y:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_IND_Y());
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_CMP_SP:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_SP());
        break;
    case MORO8_OP_CMP_SP_X:
        _moro8_cmp_ac(cpu, MORO8_GET_MEM_SP_X());
        break;
#endif
    case MORO8_OP_CPX_IMM:
        _moro8_cmp_x(cpu, operand);
        break;
    case MORO8_OP_CPX_ZP:
        _moro8_cmp_x(cpu, MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_CPX_ABS:
        _moro8_cmp_x(cpu, MORO8_GET_MEM_ABS());
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_CPX_SP:
        _moro8_cmp_x(cpu, MORO8_GET_MEM_SP());
        break;
#endif
    case MORO8_OP_CPY_IMM:
        _moro8_cmp_y(cpu, operand);
        break;
    case MORO8_OP_CPY_ZP:
        _moro8_cmp_y(cpu, MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_CPY_ABS:
        _moro8_cmp_y(cpu, MORO8_GET_MEM_ABS());
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_CPY_SP:
        _moro8_cmp_y(cpu, MORO8_GET_MEM_SP());
        break;
#endif
    case MORO8_OP_DEA:
        _moro8_dec_reg(cpu, &MORO8_AC);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_DEC_ZP:
        _moro8_dec(cpu, MORO8_ADDR_ZP);
        break;
    case MORO8_OP_DEC_ZP_X:
        _moro8_dec(cpu, MORO8_ADDR_ZP_X);
        break;
    case MORO8_OP_DEC_ABS:
        _moro8_dec(cpu, MORO8_ADDR_ABS);
        break;
    case MORO8_OP_DEC_ABS_X:
        _moro8_dec(cpu, MORO8_ADDR_ABS_X);
        break;
    case MORO8_OP_DEX:
        _moro8_dec_reg(cpu, &MORO8_X);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_DEY:
        _moro8_dec_reg(cpu, &MORO8_Y);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_EOR_IMM:
        MORO8_XOR(operand);
        break;
    case MORO8_OP_EOR_ZP:
        MORO8_XOR(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_EOR_ZP_X:
        MORO8_XOR(MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_EOR_ABS:
        MORO8_XOR(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_EOR_ABS_X:
        MORO8_XOR(MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_EOR_ABS_Y:
        MORO8_XOR(MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_EOR_IND_X:
        MORO8_XOR(MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_EOR_IND_Y:
        MORO8_XOR(MORO8_GET_MEM_IND_Y());
        break;
    case MORO8_OP_INA:
        _moro8_inc_reg(cpu, &MORO8_AC);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_INC_ZP:
        _moro8_inc(cpu, MORO8_ADDR_ZP);
        break;
    case MORO8_OP_INC_ZP_X:
        _moro8_inc(cpu, MORO8_ADDR_ZP_X);
        break;
    case MORO8_OP_INC_ABS:
        _moro8_inc(cpu, MORO8_ADDR_ABS);
        break;
    case MORO8_OP_INC_ABS_X:
        _moro8_inc(cpu, MORO8_ADDR_ABS_X);
        break;
    case MORO8_OP_INX:
        _moro8_inc_reg(cpu, &MORO8_X);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_INY:
        _moro8_inc_reg(cpu, &MORO8_Y);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_JMP_ABS:
        cpu->registers.pc = MORO8_DWORD_OPERAND - 1;
        break;
    case MORO8_OP_JMP_ABS_X:
        cpu->registers.pc = (MORO8_DWORD_OPERAND + MORO8_X) - 1;
        break;
    case MORO8_OP_JMP_IND:
    {
        moro8_udword addr = MORO8_DWORD_OPERAND;
        cpu->registers.pc = MORO8_MEMORY_DWORD(addr) - 1;
        break;
    }
    case MORO8_OP_JSR_ABS:
    {
        moro8_udword addr = MORO8_DWORD_OPERAND;
        _moro8_push_stack(cpu, MORO8_HIGH(MORO8_PC));
        _moro8_push_stack(cpu, MORO8_LOW(MORO8_PC));
        cpu->registers.pc = addr - 1;
        break;
    }
    case MORO8_OP_LDA_IMM:
        MORO8_SET_AC(operand);
        break;
    case MORO8_OP_LDA_ZP:
        MORO8_SET_AC(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_LDA_ZP_X:
        MORO8_SET_AC(MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_LDA_ABS:
        MORO8_SET_AC(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_LDA_ABS_X:
        MORO8_SET_AC(MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_LDA_ABS_Y:
        MORO8_SET_AC(MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_LDA_IND_X:
        MORO8_SET_AC(MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_LDA_IND_Y:
        MORO8_SET_AC(MORO8_GET_MEM_IND_Y());
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_LDA_SP:
        MORO8_SET_AC(MORO8_GET_MEM_SP());
        break;
    case MORO8_OP_LDA_SP_X:
        MORO8_SET_AC(MORO8_GET_MEM_SP_X());
        break;
#endif
    case MORO8_OP_LDX_IMM:
        MORO8_SET_X(operand);
        break;
    case MORO8_OP_LDX_ZP:
        MORO8_SET_X(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_LDX_ZP_Y:
        MORO8_SET_X(MORO8_GET_MEM_ZP_Y());
        break;
    case MORO8_OP_LDX_ABS:
        MORO8_SET_X(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_LDX_ABS_Y:
        MORO8_SET_X(MORO8_GET_MEM_ABS_Y());
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_LDX_SP:
        MORO8_SET_X(MORO8_GET_MEM_SP());
        break;
    case MORO8_OP_LDX_SP_Y:
        MORO8_SET_X(MORO8_GET_MEM_SP_Y());
        break;
#endif
    case MORO8_OP_LDY_IMM:
        MORO8_SET_Y(operand);
        break;
    case MORO8_OP_LDY_ZP:
        MORO8_SET_Y(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_LDY_ZP_X:
        MORO8_SET_Y(MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_LDY_ABS:
        MORO8_SET_Y(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_LDY_ABS_X:
        MORO8_SET_Y(MORO8_GET_MEM_ABS_X());
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_LDY_SP:
        MORO8_SET_Y(MORO8_GET_MEM_SP());
        break;
    case MORO8_OP_LDY_SP_X:
        MORO8_SET_Y(MORO8_GET_MEM_SP_X());
        break;
#endif
    case MORO8_OP_LSR_AC:
        _moro8_ror_ac(cpu, 0);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_LSR_ZP:
        _moro8_ror(cpu, MORO8_ADDR_ZP, 0);
        break;
    case MORO8_OP_LSR_ZP_X:
        _moro8_ror(cpu, MORO8_ADDR_ZP_X, 0);
        break;
    case MORO8_OP_LSR_ABS:
        _moro8_ror(cpu, MORO8_ADDR_ABS, 0);
        break;
    case MORO8_OP_LSR_ABS_X:
        _moro8_ror(cpu, MORO8_ADDR_ABS_X, 0);
        break;
    case MORO8_OP_NOP:
        MORO8_DEC_PC;
        break;
    case MORO8_OP_ORA_IMM:
        MORO8_OR(operand);
        break;
    case MORO8_OP_ORA_ZP:
        MORO8_OR(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_ORA_ZP_X:
        MORO8_OR(MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_ORA_ABS:
        MORO8_OR(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_ORA_ABS_X:
        MORO8_OR(MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_ORA_ABS_Y:
        MORO8_OR(MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_ORA_IND_X:
        MORO8_OR(MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_ORA_IND_Y:
        MORO8_OR(MORO8_GET_MEM_IND_Y());
        break;
    case MORO8_OP_PHA:
        _moro8_push_stack(cpu, MORO8_AC);
        MORO8_DEC_PC;
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_PHX:
        _moro8_push_stack(cpu, MORO8_X);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_PHY:
        _moro8_push_stack(cpu, MORO8_Y);
        MORO8_DEC_PC;
        break;
#endif
    case MORO8_OP_PHP:
        _moro8_push_stack(cpu, MORO8_SR);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_PLA:
        MORO8_SET_AC(_moro8_pop_stack(cpu));
        MORO8_DEC_PC;
        break;
#if MORO8_EXTENDED_OPCODES
    case MORO8_OP_PLX:
        MORO8_SET_X(_moro8_pop_stack(cpu));
        MORO8_DEC_PC;
        break;
    case MORO8_OP_PLY:
        MORO8_SET_Y(_moro8_pop_stack(cpu));
        MORO8_DEC_PC;
        break;
#endif
    case MORO8_OP_PLP:
        MORO8_SET_SR(_moro8_pop_stack(cpu));
        MORO8_DEC_PC;
        break;
    case MORO8_OP_ROL_AC:
        _moro8_rol_ac(cpu, MORO8_C);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_ROL_ZP:
        _moro8_rol(cpu, MORO8_ADDR_ZP, MORO8_C);
        break;
    case MORO8_OP_ROL_ZP_X:
        _moro8_rol(cpu, MORO8_ADDR_ZP_X, MORO8_C);
        break;
    case MORO8_OP_ROL_ABS:
        _moro8_rol(cpu, MORO8_ADDR_ABS, MORO8_C);
        break;
    case MORO8_OP_ROL_ABS_X:
        _moro8_rol(cpu, MORO8_ADDR_ABS_X, MORO8_C);
        break;
    case MORO8_OP_ROR_AC:
        _moro8_ror_ac(cpu, MORO8_C);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_ROR_ZP:
        _moro8_ror(cpu, MORO8_ADDR_ZP, MORO8_C);
        break;
    case MORO8_OP_ROR_ZP_X:
        _moro8_ror(cpu, MORO8_ADDR_ZP_X, MORO8_C);
        break;
    case MORO8_OP_ROR_ABS:
        _moro8_ror(cpu, MORO8_ADDR_ABS, MORO8_C);
        break;
    case MORO8_OP_ROR_ABS_X:
        _moro8_ror(cpu, MORO8_ADDR_ABS_X, MORO8_C);
        break;
    case MORO8_OP_RTS:
        cpu->registers.pc = _moro8_pop_stack(cpu);
        cpu->registers.pc += _moro8_pop_stack(cpu) << 8;
        break;
    case MORO8_OP_SBC_IMM:
        _moro8_sbc(cpu, operand);
        break;
    case MORO8_OP_SBC_ZP:
        _moro8_sbc(cpu, MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_SBC_ZP_X:
        _moro8_sbc(cpu, MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_SBC_ABS:
        _moro8_sbc(cpu, MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_SBC_ABS_X:
        _moro8_sbc(cpu, MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_SBC_ABS_Y:
        _moro8_sbc(cpu, MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_SBC_IND_X:
        _moro8_sbc(cpu, MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_SBC_IND_Y:
        _moro8_sbc(cpu, MORO8_GET_MEM_IND_Y());
        break;
    case MORO8_OP_SEC:
        MORO8_C = 1;
        MORO8_DEC_PC;
        break;
    case MORO8_OP_STA_ZP:
        MORO8_SET_MEM_ZP(MORO8_AC);
        break;
    case MORO8_OP_STA_ZP_X:
        MORO8_SET_MEM_ZP_X(MORO8_AC);
        break;
    case MORO8_OP_STA_ABS:
        MORO8_SET_MEM_ABS(MORO8_AC);
        break;
    case MORO8_OP_STA_ABS_X:
        MORO8_SET_MEM_ABS_X(MORO8_AC);
        break;
    case MORO8_OP_STA_ABS_Y:
        MORO8_SET_MEM_ABS_Y(MORO8_AC);
        break;
    case MORO8_OP_STA_IND_X:
        MORO8_SET_MEM_IND_X(MORO8_AC);
        break;
    case MORO8_OP_STA_IND_Y:
        MORO8_SET_MEM_IND_Y(MORO8_AC);
        break;
    case MORO8_OP_STX_ZP:
        MORO8_SET_MEM_ZP(MORO8_X);
        break;
    case MORO8_OP_STX_ZP_Y:
        MORO8_SET_MEM_ZP_Y(MORO8_X);
        break;
    case MORO8_OP_STX_ABS:
        MORO8_SET_MEM_ABS(MORO8_X);
        break;
    case MORO8_OP_STY_ZP:
        MORO8_SET_MEM_ZP(MORO8_Y);
        break;
    case MORO8_OP_STY_ZP_X:
        MORO8_SET_MEM_ZP_X(MORO8_Y);
        break;
    case MORO8_OP_STY_ABS:
        MORO8_SET_MEM_ABS(MORO8_Y);
        break;
    case MORO8_OP_TAX:
        MORO8_SET_X(MORO8_AC);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_TAY:
        MORO8_SET_Y(MORO8_AC);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_TSX:
        MORO8_SET_X(MORO8_SP);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_TXA:
        MORO8_SET_AC(MORO8_X);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_TXS:
        MORO8_SET_SP(MORO8_X);
        MORO8_DEC_PC;
        break;
    case MORO8_OP_TYA:
        MORO8_SET_AC(MORO8_Y);
        MORO8_DEC_PC;
        break;
    }

    cpu->registers.pc++;
    return MORO8_TRUE;
}

MORO8_PUBLIC(void)
moro8_run(moro8_cpu *cpu)
{
    moro8_resume(cpu);
}

MORO8_PUBLIC(moro8_udword)
moro8_get_pc(const moro8_cpu *cpu)
{
    return cpu->registers.pc;
}

MORO8_PUBLIC(void)
moro8_set_pc(moro8_cpu *cpu, moro8_udword value)
{
    cpu->registers.pc = value;
}

MORO8_PUBLIC(moro8_uword)
moro8_get_register(const moro8_cpu *cpu, moro8_register reg)
{
    switch (reg)
    {
    case MORO8_REGISTER_AC:
        return MORO8_AC;
    case MORO8_REGISTER_X:
        return MORO8_X;
    case MORO8_REGISTER_Y:
        return MORO8_Y;
    case MORO8_REGISTER_SR:
        return MORO8_SR;
    case MORO8_REGISTER_N:
        return MORO8_N;
    case MORO8_REGISTER_V:
        return MORO8_V;
    case MORO8_REGISTER_Z:
        return MORO8_Z;
    case MORO8_REGISTER_C:
        return MORO8_C;
    case MORO8_REGISTER_SP:
        return MORO8_SP;
    default:
        return 0;
    }
}

MORO8_PUBLIC(void)
moro8_set_register(moro8_cpu *cpu, enum moro8_register reg, moro8_uword value)
{
    switch (reg)
    {
    case MORO8_REGISTER_AC:
        MORO8_AC = value;
        break;
    case MORO8_REGISTER_X:
        MORO8_X = value;
        break;
    case MORO8_REGISTER_Y:
        MORO8_Y = value;
        break;
    case MORO8_REGISTER_SR:
        MORO8_SET_SR(value);
        break;
    case MORO8_REGISTER_N:
        MORO8_N = value;
        break;
    case MORO8_REGISTER_V:
        MORO8_V = value;
        break;
    case MORO8_REGISTER_Z:
        MORO8_Z = value;
        break;
    case MORO8_REGISTER_C:
        MORO8_C = value;
        break;
    case MORO8_REGISTER_SP:
        MORO8_SP = value;
        break;
    default:
        break;
    }
}

MORO8_PUBLIC(moro8_udword)
moro8_get_memory(const moro8_cpu *cpu, moro8_uword *buffer, moro8_udword offset, moro8_udword size)
{
    if (cpu->memory)
    {
        return cpu->memory->get(cpu->memory, buffer, offset, size);
    }

    return 0;
}

MORO8_PUBLIC(moro8_udword)
moro8_set_memory(moro8_cpu *cpu, const moro8_uword *buffer, moro8_udword offset, moro8_udword size)
{
    if (cpu->memory)
    {
        return cpu->memory->set(cpu->memory, buffer, offset, size);
    }

    return 0;
}

MORO8_PUBLIC(void)
moro8_set_memory_bus(moro8_cpu *cpu, moro8_bus *bus)
{
    cpu->memory = bus;
}

MORO8_PUBLIC(moro8_bus *)
moro8_get_memory_bus(moro8_cpu *cpu)
{
    return cpu->memory;
}

#if MORO8_WITH_PARSER

/** Print a single 0-15 value as a single hex character */
static inline void moro8_print_hex(char value, char **buf, size_t *size)
{
    if (*size >= 1)
    {
        (*size)--;
        *buf[0] = (char)((value >= 0 && value <= 9) ? ('0' + value) : ('A' + (value - 10)));
    }

    (*buf)++;
}

/** Print a single word as 00 */
static inline void moro8_print_word(moro8_uword value, char **buf, size_t *size)
{
    moro8_print_hex((value & 0xF0) >> 4, buf, size);
    moro8_print_hex(value & 0xF, buf, size);
}

static inline void moro8_print_char(char value, char **buf, size_t *size)
{
    if (*size >= 1)
    {
        (*size)--;
        *buf[0] = value;
    }

    (*buf)++;
}

static inline void moro8_print_string(const char *value, char **buf, size_t *size)
{
    size_t written = snprintf(*buf, *size, "%s", value);

    if (written > *size)
    {
        *size = 0;
    }
    else
    {
        *size -= written;
    }

    *buf += written;
}

/** Print a double word as LL HH */
static inline void moro8_print_dword(moro8_udword value, char **buf, size_t *size)
{
    moro8_print_word((moro8_uword)MORO8_LOW(value), buf, size);
    moro8_print_char(' ', buf, size);
    moro8_print_word((moro8_uword)MORO8_HIGH(value), buf, size);
}

/** Print REG: 00\n */
static inline void moro8_print_register_word(const char *name, moro8_uword value, char **buf, size_t *size)
{
    moro8_print_string(name, buf, size);
    moro8_print_char(' ', buf, size);
    moro8_print_word(value, buf, size);
    moro8_print_char('\n', buf, size);
}

/** Print REG: LL HH\n */
static inline void moro8_print_register_dword(const char *name, moro8_udword value, char **buf, size_t *size)
{

    moro8_print_string(name, buf, size);
    moro8_print_char(' ', buf, size);
    moro8_print_dword(value, buf, size);
    moro8_print_char('\n', buf, size);
}

/** Print the whole memory as 0000: 00 00 00 00 ... 00 00 00 00 */
static inline void moro8_print_memory(const moro8_cpu *cpu, char **buf, size_t *size)
{
    if (!cpu->memory)
    {
        return;
    }

    moro8_udword address = 0;
    for (moro8_udword i = 0; i < MORO8_MEMORY_SIZE - 0x10; i += 0x10)
    {
        if (i > 0)
        {
            moro8_print_char('\n', buf, size);
        }

        moro8_print_word((moro8_uword)MORO8_HIGH(i), buf, size);
        moro8_print_word((moro8_uword)MORO8_LOW(i), buf, size);
        moro8_print_char(':', buf, size);
        moro8_print_char(' ', buf, size);

        for (moro8_uword j = 0; j <= 0xF; ++j)
        {
            if (j > 0)
            {
                moro8_print_char(' ', buf, size);
                if ((j % 4) == 0)
                {
                    moro8_print_char(' ', buf, size);
                }
            }
            moro8_print_word(MORO8_GET_MEM(address++), buf, size);
        }
    }
}

MORO8_PUBLIC(size_t)
moro8_print(const struct moro8_cpu *cpu, char *buf, size_t size)
{
    char **buf_ptr = &buf;
    size_t *size_ptr = &size;
    moro8_print_register_dword("PC:", cpu->registers.pc, buf_ptr, size_ptr);
    moro8_print_register_word("AC:", cpu->registers.ac, buf_ptr, size_ptr);
    moro8_print_register_word("X: ", cpu->registers.x, buf_ptr, size_ptr);
    moro8_print_register_word("Y: ", cpu->registers.y, buf_ptr, size_ptr);
    moro8_print_register_word("SP:", cpu->registers.sp, buf_ptr, size_ptr);
    moro8_print_register_word("N: ", cpu->registers.sr.n, buf_ptr, size_ptr);
    moro8_print_register_word("V: ", cpu->registers.sr.v, buf_ptr, size_ptr);
    moro8_print_register_word("Z: ", cpu->registers.sr.z, buf_ptr, size_ptr);
    moro8_print_register_word("C: ", cpu->registers.sr.c, buf_ptr, size_ptr);
    moro8_print_char('\n', buf_ptr, size_ptr);
    moro8_print_memory(cpu, buf_ptr, size_ptr);
    moro8_print_char('\0', buf_ptr, size_ptr);

    return MORO8_PRINT_BUFFER_SIZE - 1;
}

#define MORO8_IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))

MORO8_PUBLIC(moro8_uword)
moro8_parse_hex(char value)
{
    switch (value)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return value - '0';
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return value - 'a' + 10;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return value - 'A' + 10;
    default:
        return 0;
    }
}

MORO8_PUBLIC(size_t)
moro8_parse_word(const char *buf, size_t size, moro8_uword *value)
{
    *value = 0;
    char c;
    size_t read = 0;
    size_t col = 0;
    for (size_t i = 0; i < size && col < 2; ++i)
    {
        c = *buf;
        read++;
        switch (c)
        {
        case '\0':
            return 0;
        case ' ':
        case '\t':
        case '\n':
            break;
        default:
            if (!MORO8_IS_HEX(c))
            {
                return 0;
            }

            *value += moro8_parse_hex(c) << (((col + 1) % 2) * 4);
            col++;
            if (col == 2)
            {
                return read;
            }
            break;
        }

        buf++;
    }

    return 0;
}

MORO8_PUBLIC(size_t)
moro8_parse_dword(const char *buf, size_t size, moro8_udword *value)
{
    *value = 0;
    size_t read = 0;
    size_t result;
    moro8_uword word;
    for (size_t i = 0; i < 2; ++i)
    {
        if ((result = moro8_parse_word(buf, size, &word)) == 0)
        {
            return 0;
        }

        read += result;
        buf += result;
        size -= result;
        *value += ((moro8_udword)word) << (((i + 1) % 2) * 8);
    }

    return read;
}

/** Parse a HHLL hex string to double word */
static inline int moro8_parse_address(const char *buf, moro8_udword *value)
{
    if (!(MORO8_IS_HEX(buf[0]) && MORO8_IS_HEX(buf[1]) && MORO8_IS_HEX(buf[2]) && MORO8_IS_HEX(buf[3])))
    {
        return MORO8_FALSE;
    }

    *value = (((moro8_parse_hex(buf[0]) << 4) + moro8_parse_hex(buf[1])) << 8) + ((moro8_parse_hex(buf[2]) << 4) + moro8_parse_hex(buf[3]));
    return MORO8_TRUE;
}

MORO8_PUBLIC(moro8_cpu *)
moro8_parse(moro8_cpu *cpu, const char *buf, size_t size)
{
#define MORO8_IS_LETTER_OR_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z'))
#define MORO8_STATE_IDLE 0
#define MORO8_STATE_VALUE 1
#define MORO8_STATE_COMMENT 2
#define MORO8_SUBSTATE_REGISTER 0
#define MORO8_SUBSTATE_SR 1
#define MORO8_SUBSTATE_MEMORY 2

    _moro8_reset(cpu);
    // When reading status register
    moro8_uword sr = 0;
    moro8_uword sr_flag = 0;
    // Pointer to the moro8_uword we are reading
    moro8_uword *value_buffer = NULL;
    // Used when reading memory values
    moro8_udword base_address = 0;
    moro8_udword address = 0;
    moro8_udword value = 0;
    size_t value_index = 0;
    // State and substate
    int state = MORO8_STATE_IDLE;
    int substate = MORO8_SUBSTATE_REGISTER;
    // Current character
    char c = 0;
    size_t line = 1;
    size_t col = 1;

    for (size_t i = 0; i < size;)
    {
        c = *buf;

        // Consume whitespaces and newlines
        switch (c)
        {
        case '\0':
            return cpu;
        case '#':
            state = MORO8_STATE_COMMENT;
        case ' ':
        case '\t':
        case '\r':
            ++col;
            ++i;
            ++buf;
            continue;
        case '\n':
            state = MORO8_STATE_IDLE;
            line++;
            col = 1;
            ++i;
            ++buf;
            continue;
        default:
            // Skip comments
            if (state == MORO8_STATE_COMMENT)
            {
                ++col;
                ++i;
                ++buf;
                continue;
            }
            break;
        }

        if (state == MORO8_STATE_IDLE)
        {
            // Idle state, search for label
            if (!MORO8_IS_LETTER_OR_HEX(c))
            {
                printf("%zd.%zd: encountered invalid character %c\n", line, col, c);
                return NULL;
            }

            // Search for next : following the label
            for (size_t j = 0; i < size; ++j, ++i)
            {
                if (buf[j] == ':')
                {
                    // End of label
                    if (j == 4 && moro8_parse_address(buf, &base_address))
                    {
                        substate = MORO8_SUBSTATE_MEMORY;
                        value_buffer = NULL;
                    }
                    else if (buf[0] == 'P' && buf[1] == 'C')
                    {
                        substate = MORO8_SUBSTATE_REGISTER;
                        cpu->registers.pc = 0;
                        value_buffer = (moro8_uword *)&cpu->registers.pc;
                    }
                    else if (buf[0] == 'A' && buf[1] == 'C')
                    {
                        substate = MORO8_SUBSTATE_REGISTER;
                        value_buffer = &cpu->registers.ac;
                    }
                    else if (buf[0] == 'X')
                    {
                        substate = MORO8_SUBSTATE_REGISTER;
                        value_buffer = &cpu->registers.x;
                    }
                    else if (buf[0] == 'Y')
                    {
                        substate = MORO8_SUBSTATE_REGISTER;
                        value_buffer = &cpu->registers.y;
                    }
                    else if (buf[0] == 'S' && buf[1] == 'P')
                    {
                        substate = MORO8_SUBSTATE_REGISTER;
                        value_buffer = &cpu->registers.sp;
                    }
                    else if (buf[0] == 'N')
                    {
                        substate = MORO8_SUBSTATE_SR;
                        value_buffer = &sr;
                        sr = 0;
                        sr_flag = 0x80;
                    }
                    else if (buf[0] == 'V')
                    {
                        substate = MORO8_SUBSTATE_SR;
                        value_buffer = &sr;
                        sr = 0;
                        sr_flag = 0x40;
                    }
                    else if (buf[0] == 'C')
                    {
                        substate = MORO8_SUBSTATE_SR;
                        value_buffer = &sr;
                        sr = 0;
                        sr_flag = 0x1;
                    }
                    else if (buf[0] == 'Z')
                    {
                        substate = MORO8_SUBSTATE_SR;
                        value_buffer = &sr;
                        sr = 0;
                        sr_flag = 0x2;
                    }
                    else
                    {
                        // Unknown label
                        printf("%zd.%zd: encountered unknown label %.*s\n", line, col + j, (int)j, buf);
                        return NULL;
                    }

                    // Now go parse the value
                    ++i;
                    buf += j + 1;
                    col += j + 1;
                    state = MORO8_STATE_VALUE;
                    value_index = 0;
                    break;
                }
                else if (!MORO8_IS_LETTER_OR_HEX(buf[j]))
                {
                    // Invalid character while parsing label
                    printf("%zd.%zd: encountered invalid character %c\n", line, col + j, buf[j]);
                    return NULL;
                }
            }

            if (state == MORO8_STATE_IDLE)
            {
                // Couldn't find next : character
                printf("%zd.%zd: character : not found after label\n", line, col);
                return NULL;
            }
        }
        else if (state == MORO8_STATE_VALUE)
        {
            // Value state, fill memory
            if (!MORO8_IS_HEX(c))
            {
                // Values are only 0-9A-F
                printf("%zd.%zd: encountered invalid character %c\n", line, col, c);
                return NULL;
            }

            value = moro8_parse_hex(c) << (((value_index + 1) % 2) * 4);

            if (value_buffer)
            {
                // Storing in register
                if (value_index % 2 == 0)
                {
                    value_buffer[(moro8_udword)value_index / 2] = (moro8_uword)value;
                }
                else
                {
                    value_buffer[(moro8_udword)value_index / 2] += (moro8_uword)value;
                }

                if (substate == MORO8_SUBSTATE_SR)
                {
                    // Toggle corresponding flag in status register
                    MORO8_SET_SR((MORO8_SR & ~sr_flag) + (sr != 0 ? sr_flag : 0));
                }
            }
            else if (cpu->memory)
            {
                // Storing in memory
                address = (moro8_udword)(base_address + value_index / 2);
                cpu->memory->set_word(cpu->memory, address, cpu->memory->get_word(cpu->memory, address) + value);
            }

            ++value_index;
            ++buf;
            ++col;
        }
    }

    return cpu;

#undef MORO8_IS_HEX
#undef MORO8_IS_LETTER_OR_HEX
#undef MORO8_STATE_IDLE
#undef MORO8_STATE_VALUE
#undef MORO8_STATE_COMMENT
#undef MORO8_IS_ADDRESS
}

#endif /* !MORO8_WITH_PARSER */

#if MORO8_WITH_HANDLERS

MORO8_PUBLIC(moro8_handler)
moro8_get_handler(moro8_cpu *cpu, moro8_uword op)
{
    return cpu->handlers[op];
}

MORO8_PUBLIC(void)
moro8_set_handler(moro8_cpu *cpu, moro8_uword op, moro8_handler handler)
{
    cpu->handlers[op] = handler;
}

#endif

#if !MORO8_MINIMALIST

MORO8_PUBLIC(moro8_udword)
moro8_memory_get(const struct moro8_bus *memory, moro8_uword *buffer, moro8_udword offset, moro8_udword size)
{
    return memory->get(memory, buffer, offset, size);
}

MORO8_PUBLIC(moro8_udword)
moro8_memory_set(struct moro8_bus *memory, const moro8_uword *buffer, moro8_udword offset, moro8_udword size)
{
    return memory->set(memory, buffer, offset, size);
}

MORO8_PUBLIC(moro8_uword)
moro8_memory_get_word(const struct moro8_bus *memory, moro8_udword address)
{
    return memory->get_word(memory, address);
}

MORO8_PUBLIC(void)
moro8_memory_set_word(struct moro8_bus *memory, moro8_udword address, moro8_uword value)
{
    memory->set_word(memory, address, value);
}

MORO8_PUBLIC(void)
moro8_memory_reset(struct moro8_bus *memory)
{
    memory->reset(memory);
}

MORO8_PUBLIC(moro8_udword)
moro8_memory_get_dword(const struct moro8_bus *memory, moro8_udword address)
{
    return memory->get_dword(memory, address);
}

MORO8_PUBLIC(void)
moro8_memory_set_dword(struct moro8_bus *memory, moro8_udword address, moro8_udword value)
{
    memory->set_dword(memory, address, value);
}

MORO8_PUBLIC(moro8_uword)
moro8_get_memory_word(struct moro8_cpu *cpu, moro8_udword address)
{
    if (cpu->memory)
    {
        return cpu->memory->get_word(cpu->memory, address);
    }

    return 0;
}

MORO8_PUBLIC(void)
moro8_set_memory_word(struct moro8_cpu *cpu, moro8_udword address, moro8_uword value)
{
    if (cpu->memory)
    {
        cpu->memory->set_word(cpu->memory, address, value);
    }
}

MORO8_PUBLIC(moro8_udword)
moro8_get_memory_dword(struct moro8_cpu *cpu, moro8_udword address)
{
    if (cpu->memory)
    {
        return cpu->memory->get_dword(cpu->memory, address);
    }

    return 0;
}

MORO8_PUBLIC(void)
moro8_set_memory_dword(struct moro8_cpu *cpu, moro8_udword address, moro8_udword value)
{
    if (cpu->memory)
    {
        cpu->memory->set_dword(cpu->memory, address, value);
    }
}

MORO8_PUBLIC(void)
moro8_copy(moro8_cpu *snapshot, const moro8_cpu *cpu)
{
    memcpy(&snapshot->registers, &cpu->registers, sizeof(moro8_registers));

    if (!snapshot->memory || !cpu->memory)
    {
        return;
    }

    for (moro8_udword i = _MORO8_MAX_ADDR; i >= 0; --i)
    {
        snapshot->memory->set_word(snapshot->memory, i, cpu->memory->get_word(cpu->memory, i));

        if (i == 0)
        {
            break;
        }
    }
}

MORO8_PUBLIC(int)
moro8_equal(const moro8_cpu *left, const moro8_cpu *right)
{
    if (left == right)
    {
        return MORO8_TRUE;
    }

    if (!left || !right)
    {
        return MORO8_FALSE;
    }

    if (memcmp(&left->registers, &right->registers, sizeof(moro8_registers)) != 0)
    {
        return MORO8_FALSE;
    }

    if (left->memory == right->memory)
    {
        return MORO8_TRUE;
    }

    if (!left->memory || !right->memory)
    {
        return MORO8_FALSE;
    }

    for (moro8_udword i = _MORO8_MAX_ADDR; i >= 0; --i)
    {
        if (left->memory->get_word(left->memory, i) != right->memory->get_word(right->memory, i))
        {
            return MORO8_FALSE;
        }

        if (i == 0)
        {
            break;
        }
    }

    return MORO8_TRUE;
}

#endif

#ifdef MORO8_DOXYGEN

/** Get registers values. */
#define _moro8_get_ac(cpu) moro8_get_register(cpu, MORO8_REGISTER_AC)
#define _moro8_get_x(cpu) moro8_get_register(cpu, MORO8_REGISTER_X)
#define _moro8_get_y(cpu) moro8_get_register(cpu, MORO8_REGISTER_Y)
#define _moro8_get_sp(cpu) moro8_get_register(cpu, MORO8_REGISTER_SP)
#define _moro8_get_sr(cpu) moro8_get_register(cpu, MORO8_REGISTER_SR)
#define _moro8_get_c(cpu) moro8_get_register(cpu, MORO8_REGISTER_C)
#define _moro8_get_v(cpu) moro8_get_register(cpu, MORO8_REGISTER_V)
#define _moro8_get_n(cpu) moro8_get_register(cpu, MORO8_REGISTER_N)
#define _moro8_get_z(cpu) moro8_get_register(cpu, MORO8_REGISTER_Z)
/** Set registers values. */
#define _moro8_set_ac(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_AC, value)
#define _moro8_set_x(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_X, value)
#define _moro8_set_y(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_Y, value)
#define _moro8_set_sp(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_SP, value)
#define _moro8_set_sr(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_SR, value)
#define _moro8_set_c(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_C, value)
#define _moro8_set_v(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_V, value)
#define _moro8_set_n(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_N, value)
#define _moro8_set_z(cpu, value) moro8_set_register(cpu, MORO8_REGISTER_Z, value)

MORO8_PUBLIC(moro8_uword)
moro8_get_ac(struct moro8_cpu *cpu)
{
    return _moro8_get_ac(cpu);
}
MORO8_PUBLIC(moro8_uword)
moro8_get_x(struct moro8_cpu *cpu) { return _moro8_get_x(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_y(struct moro8_cpu *cpu) { return _moro8_get_y(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_sp(struct moro8_cpu *cpu) { return _moro8_get_sp(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_sr(struct moro8_cpu *cpu) { return _moro8_get_sr(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_c(struct moro8_cpu *cpu) { return _moro8_get_c(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_v(struct moro8_cpu *cpu) { return _moro8_get_v(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_n(struct moro8_cpu *cpu) { return _moro8_get_n(cpu); }
MORO8_PUBLIC(moro8_uword)
moro8_get_z(struct moro8_cpu *cpu) { return _moro8_get_z(cpu); }
MORO8_PUBLIC(void)
moro8_set_ac(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_ac(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_x(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_x(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_y(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_y(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_sp(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_sp(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_sr(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_sr(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_c(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_c(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_v(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_v(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_n(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_n(cpu, value); }
MORO8_PUBLIC(void)
moro8_set_z(struct moro8_cpu *cpu, moro8_uword value) { _moro8_set_z(cpu, value); }

#endif
