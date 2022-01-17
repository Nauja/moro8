#include "moro8.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#define MORO8_TRUE 1
#define MORO8_FALSE 0

#define MORO8_PC vm->registers.pc
#define MORO8_AC vm->registers.ac
#define MORO8_X vm->registers.x
#define MORO8_Y vm->registers.y
#define MORO8_SR vm->registers.sr
#define MORO8_SP vm->registers.sp
#define MORO8_C vm->registers.sr.c
#define MORO8_Z vm->registers.sr.z
#define MORO8_V vm->registers.sr.v
#define MORO8_N vm->registers.sr.n
#define MORO8_GET_MEM(addr) vm->memory->get(vm->memory, addr)
#define MORO8_SET_MEM(addr, value) vm->memory->set(vm->memory, addr, value)

typedef struct moro8_registers moro8_registers;
typedef struct moro8_vm moro8_vm;
typedef struct moro8_array_memory moro8_array_memory;
typedef enum moro8_register moro8_register;

moro8_array_memory* moro8_array_memory_create()
{
    moro8_array_memory* memory = (moro8_array_memory*)MORO8_MALLOC(sizeof(moro8_array_memory));
    moro8_array_memory_init(memory);
    return memory;
}

void moro8_array_memory_init(moro8_array_memory* memory)
{
    memory->bus.get = &moro8_array_memory_get;
    memory->bus.set = &moro8_array_memory_set;
}

void moro8_array_memory_delete(moro8_array_memory* memory)
{
    free(memory);
}

moro8_uword moro8_array_memory_get(const struct moro8_array_memory* memory, moro8_udword address)
{
    return memory->buffer[address];
}

void moro8_array_memory_set(struct moro8_array_memory* memory, moro8_udword address, moro8_uword value)
{
    memory->buffer[address] = value;
}

moro8_vm* moro8_create()
{
    return (moro8_vm*)(malloc(sizeof(moro8_vm)));
}

void moro8_init(moro8_vm* vm)
{
    memset(vm, 0, sizeof(moro8_vm));
}

void moro8_reset(moro8_vm* vm)
{
    memset(&vm->registers, 0, sizeof(moro8_registers));
}

const void* moro8_as_buffer(const moro8_vm* vm, size_t* size)
{
    return NULL;
}

void moro8_from_buffer(moro8_vm* vm, const void* buf, size_t size)
{
}

void moro8_backup(moro8_vm* snapshot, const moro8_vm* vm)
{
    memcpy(&snapshot->registers, &vm->registers, sizeof(moro8_registers));
    for (moro8_udword i = 0; i < MORO8_MEMORY_SIZE; ++i)
    {
        snapshot->memory->set(snapshot->memory, i, vm->memory->get(vm->memory, i));
    }
}

void moro8_restore(moro8_vm* vm, const moro8_vm* snapshot)
{
    memcpy(&vm->registers, &snapshot->registers, sizeof(moro8_registers));
    for (moro8_udword i = 0; i < MORO8_MEMORY_SIZE; ++i)
    {
        vm->memory->set(vm->memory, i, snapshot->memory->get(snapshot->memory, i));
    }
}

int moro8_equal(const moro8_vm* left, const moro8_vm* right)
{
    if (left == right)
    {
        return MORO8_TRUE;
    }

    if (left == NULL || right == NULL)
    {
        return MORO8_FALSE;
    }

    if (memcmp(&left->registers, &right->registers, sizeof(moro8_registers)) != 0)
    {
        return MORO8_FALSE;
    }

    for (moro8_udword i = 0; i < MORO8_MEMORY_SIZE; ++i)
    {
        if (left->memory->get(left->memory, i) != right->memory->get(right->memory, i))
        {
            return MORO8_FALSE;
        }
    }

    return MORO8_TRUE;
}

void moro8_load(moro8_vm* vm, const moro8_uword* prog, moro8_udword size)
{
    moro8_reset(vm);
    moro8_set_memory(vm, prog, MORO8_ROM_OFFSET, size);
}

void moro8_resume(moro8_vm* vm)
{
    while(moro8_step(vm))
    {
    }
}

size_t moro8_step(moro8_vm* vm)
{
    moro8_uword instruction = MORO8_GET_MEM(vm->registers.pc);

#if MORO8_WITH_HOOKS
    // Lets custom hooks handle the instruction
    if (vm->hooks[instruction] && vm->hooks[instruction](vm, instruction))
    {
        return MORO8_TRUE;
    }
#endif

    moro8_uword operand = MORO8_GET_MEM(++vm->registers.pc);

/** Get a double word operand */
#define MORO8_DWORD_OPERAND (operand + (MORO8_GET_MEM(++vm->registers.pc) << 8))
/** Get a double word at a memory address */
#define MORO8_MEMORY_DWORD(addr) ((moro8_udword)MORO8_GET_MEM(addr) + ((addr < MORO8_MEMORY_SIZE - 1) ? (((moro8_udword)MORO8_GET_MEM(addr + 1)) << 8) : 0))
#define MORO8_MEMORY_SET_DWORD(addr, value) MORO8_SET_MEM(addr, MORO8_LOW(value)); if (addr < MORO8_MEMORY_SIZE - 1) { MORO8_SET_MEM(addr + 1, MORO8_HIGH(value)); }
#define MORO8_DEC_PC --vm->registers.pc
#define MORO8_TWO_COMPLEMENT(value) (~result + 1)
    /** Build address depending on addressing mode. */
#define MORO8_ADDR_ZP operand
#define MORO8_ADDR_ZP_X (operand + MORO8_X)
#define MORO8_ADDR_ZP_Y (operand + MORO8_Y)
#define MORO8_ADDR_ABS MORO8_DWORD_OPERAND
#define MORO8_ADDR_ABS_X (MORO8_DWORD_OPERAND + MORO8_X)
#define MORO8_ADDR_ABS_Y (MORO8_DWORD_OPERAND + MORO8_Y)
#define MORO8_ADDR_IND_X (MORO8_MEMORY_DWORD(operand + MORO8_X))
#define MORO8_ADDR_IND_Y (MORO8_MEMORY_DWORD(operand) + MORO8_Y)
    /** Get from memory depending on addressing mode. */
#define MORO8_GET_MEM_ZP() MORO8_GET_MEM(MORO8_ADDR_ZP)
#define MORO8_GET_MEM_ZP_X() MORO8_GET_MEM(MORO8_ADDR_ZP_X)
#define MORO8_GET_MEM_ZP_Y() MORO8_GET_MEM(MORO8_ADDR_ZP_Y)
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
#define MORO8_SET_X(value) \
    MORO8_X = value; \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_X); \
    MORO8_Z = MORO8_X == 0
#define MORO8_SET_Y(value) \
    MORO8_Y = value; \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_Y); \
    MORO8_Z = MORO8_Y == 0
#define MORO8_SET_AC(value) \
    MORO8_AC = value; \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_AC); \
    MORO8_Z = MORO8_AC == 0
#define MORO8_SET_SP(value) MORO8_SP = value
#define MORO8_ADC(operand) \
{ \
    moro8_uword value = operand; \
    moro8_uword sign_bit = MORO8_SIGN(MORO8_AC) == MORO8_SIGN(value); \
    moro8_udword result = MORO8_AC + value + MORO8_C; \
    MORO8_C = result > 0xFF; \
    MORO8_SET_AC((moro8_uword)(result & 0xFF)); \
    MORO8_V = sign_bit && (MORO8_SIGN(MORO8_AC) != MORO8_SIGN(value)); \
}
#define MORO8_AND(operand) MORO8_SET_AC(MORO8_AC & operand)
#define MORO8_OR(operand) MORO8_SET_AC(MORO8_AC | operand)
#define MORO8_XOR(operand) MORO8_SET_AC(MORO8_AC ^ operand)
#define MORO8_ASL_AC() \
{ \
    MORO8_C = MORO8_IS_NEGATIVE(MORO8_AC); \
    MORO8_AC = (MORO8_AC & 0x7F) << 1; \
    MORO8_N = MORO8_IS_NEGATIVE(MORO8_AC); \
    MORO8_Z = MORO8_AC == 0; \
}
#define MORO8_ASL(operand) \
{ \
    moro8_udword address = operand; \
    moro8_uword value = MORO8_GET_MEM(address); \
    MORO8_C = MORO8_IS_NEGATIVE(value); \
    value = (value & 0x7F) << 1; \
    MORO8_N = MORO8_IS_NEGATIVE(value); \
    MORO8_Z = value == 0; \
    MORO8_SET_MEM(address, value); \
}
#define MORO8_LSR_AC() \
{ \
    MORO8_C = MORO8_AC & 0x1; \
    MORO8_N = 0; \
    MORO8_AC = (MORO8_AC & 0xFE) >> 1; \
    MORO8_Z = MORO8_AC == 0; \
}
#define MORO8_LSR(operand) \
{ \
    moro8_udword address = operand; \
    moro8_uword value = MORO8_GET_MEM(address); \
    MORO8_C = value & 0x1; \
    MORO8_N = 0; \
    value = (value & 0xFE) >> 1; \
    MORO8_Z = value == 0; \
    MORO8_SET_MEM(address, value); \
}
#define MORO8_TRANSFER(from, to) \
{ \
    to = from; \
    MORO8_N = MORO8_IS_NEGATIVE(to); \
    MORO8_Z = to == 0; \
}
#define MORO8_BRANCH(condition) \
{ \
    if (condition) { \
        MORO8_PC += ((moro8_word)operand) - 1; \
    } \
    MORO8_DEC_PC; \
}

    if (instruction == 0 || instruction == 0x60)
    {
        return MORO8_FALSE;
    }

    switch (instruction)
    {
    case MORO8_OP_ADC_IMM:
        MORO8_ADC(operand);
        break;
    case MORO8_OP_ADC_ZP:
        MORO8_ADC(MORO8_GET_MEM_ZP());
        break;
    case MORO8_OP_ADC_ZP_X:
        MORO8_ADC(MORO8_GET_MEM_ZP_X());
        break;
    case MORO8_OP_ADC_ABS:
        MORO8_ADC(MORO8_GET_MEM_ABS());
        break;
    case MORO8_OP_ADC_ABS_X:
        MORO8_ADC(MORO8_GET_MEM_ABS_X());
        break;
    case MORO8_OP_ADC_ABS_Y:
        MORO8_ADC(MORO8_GET_MEM_ABS_Y());
        break;
    case MORO8_OP_ADC_IND_X:
        MORO8_ADC(MORO8_GET_MEM_IND_X());
        break;
    case MORO8_OP_ADC_IND_Y:
        MORO8_ADC(MORO8_GET_MEM_IND_Y());
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
        MORO8_ASL_AC();
        MORO8_DEC_PC;
        break;
    case MORO8_OP_ASL_ZP:
        MORO8_ASL(MORO8_ADDR_ZP);
        break;
    case MORO8_OP_ASL_ZP_X:
        MORO8_ASL(MORO8_ADDR_ZP_X);
        break;
    case MORO8_OP_ASL_ABS:
        MORO8_ASL(MORO8_ADDR_ABS);
        break;
    case MORO8_OP_ASL_ABS_X:
        MORO8_ASL(MORO8_ADDR_ABS_X);
        break;
    case MORO8_OP_BCC:
        MORO8_BRANCH(MORO8_C == 0);
        break;
    case MORO8_OP_BCS:
        MORO8_BRANCH(MORO8_C != 0);
        break;
    case MORO8_OP_BEQ:
        MORO8_BRANCH(MORO8_Z != 0);
        break;
    case MORO8_OP_BMI:
        MORO8_BRANCH(MORO8_N != 0);
        break;
    case MORO8_OP_BNE:
        MORO8_BRANCH(MORO8_Z == 0);
        break;
    case MORO8_OP_BPL:
        MORO8_BRANCH(MORO8_N == 0);
        break;
    case MORO8_OP_BVC:
        MORO8_BRANCH(MORO8_V == 0);
        break;
    case MORO8_OP_BVS:
        MORO8_BRANCH(MORO8_V != 0);
        break;
    case MORO8_OP_CLC:
        MORO8_C = 0;
        MORO8_DEC_PC;
        break;
    case MORO8_OP_CLV:
        MORO8_V = 0;
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
    case MORO8_OP_JMP_ABS:
        vm->registers.pc = MORO8_DWORD_OPERAND - 1;
        break;
    case MORO8_OP_JMP_IND:
    {
        moro8_udword addr = MORO8_DWORD_OPERAND;
        vm->registers.pc = MORO8_MEMORY_DWORD(addr) - 1;
        break;
    }
    case MORO8_OP_JSR_ABS:
        // @todo
        break;
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
    case MORO8_OP_LSR_AC:
        MORO8_LSR_AC();
        MORO8_DEC_PC;
        break;
    case MORO8_OP_LSR_ZP:
        MORO8_LSR(MORO8_ADDR_ZP);
        break;
    case MORO8_OP_LSR_ZP_X:
        MORO8_LSR(MORO8_ADDR_ZP_X);
        break;
    case MORO8_OP_LSR_ABS:
        MORO8_LSR(MORO8_ADDR_ABS);
        break;
    case MORO8_OP_LSR_ABS_X:
        MORO8_LSR(MORO8_ADDR_ABS_X);
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
        MORO8_SET_MEM(vm->registers.sp, MORO8_AC);
        vm->registers.sp++;
        break;
    case MORO8_OP_PLA:
        vm->registers.sp--;
        MORO8_SET_AC(MORO8_GET_MEM(vm->registers.sp));
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

    vm->registers.pc++;
    return MORO8_TRUE;
}

void moro8_run(moro8_vm* vm)
{
    vm->registers.pc = MORO8_ROM_OFFSET;
    moro8_resume(vm);
}

moro8_udword moro8_get_pc(const moro8_vm* vm)
{
    return vm->registers.pc;
}

void moro8_set_pc(moro8_vm* vm, moro8_udword value)
{
    vm->registers.pc = value;
}

moro8_uword moro8_get_register(const moro8_vm* vm, moro8_register reg)
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
        return (
            ((MORO8_N & 0x1) << 7) +
            ((MORO8_V & 0x1) << 6) +
            ((MORO8_Z & 0x1) << 1) +
            (MORO8_C & 0x1)
        );
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

void moro8_set_register(moro8_vm* vm, enum moro8_register reg, moro8_uword value)
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
        MORO8_N = (value & 0x80) != 0;
        MORO8_V = (value & 0x40) != 0;
        MORO8_Z = (value & 0x2) != 0;
        MORO8_C = (value & 0x1);
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

moro8_udword moro8_get_memory(const moro8_vm* vm, moro8_uword* buffer, moro8_udword offset, moro8_udword size)
{
    buffer[0] = 8;
    moro8_udword max_size = MORO8_MEMORY_SIZE - offset;
    if (size > max_size)
    {
        size = max_size;
    }

    if (size <= 0)
    {
        return 0;
    }

    for (moro8_udword i = offset, j = 0; j < max_size; ++i, ++j)
    {
        buffer[j] = vm->memory->get(vm->memory, i);
    }
    return size;
}

moro8_udword moro8_set_memory(moro8_vm* vm, const moro8_uword* buffer, moro8_udword offset, moro8_udword size)
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

    for (moro8_udword i = offset, j = 0; j < max_size; ++i, ++j)
    {
        vm->memory->set(vm->memory, i, buffer[j]);
    }
    return size;
}

moro8_uword moro8_get_memory_word(struct moro8_vm* vm, moro8_udword address)
{
    return vm->memory->get(vm->memory, address);
}

void moro8_set_memory_word(struct moro8_vm* vm, moro8_udword address, moro8_uword value)
{
    vm->memory->set(vm->memory, address, value);
}

moro8_udword moro8_get_memory_dword(struct moro8_vm* vm, moro8_udword address)
{
    return MORO8_MEMORY_DWORD(address);
}

void moro8_set_memory_dword(struct moro8_vm* vm, moro8_udword address, moro8_udword value)
{
    MORO8_MEMORY_SET_DWORD(address, value);
}

void moro8_delete(moro8_vm* vm)
{
    free(vm);
}

#if MORO8_WITH_PARSER

/** Print a single 0-15 value as a single hex character */
static inline char* moro8_print_hex(char* buf, char value)
{
    *buf++ = (value >= 0 && value <= 9) ? ('0' + value) : ('A' + (value - 10));
    return buf;
}

/** Print a single word as 00 */
static inline char* moro8_print_word(char* buf, moro8_uword value)
{
    buf = moro8_print_hex(buf, (value & 0xF0) >> 4);
    return moro8_print_hex(buf, value & 0xF);
}

/** Print a double word as LL HH */
static inline char* moro8_print_dword(char* buf, moro8_udword value)
{
    buf = moro8_print_word(buf, (moro8_uword)MORO8_LOW(value));
    *buf++ = ' ';
    return moro8_print_word(buf, (moro8_uword)MORO8_HIGH(value));
}

/** Print REG: 00\n */
static inline char* moro8_print_register_word(char* buf, const char* name, moro8_uword value)
{
    buf += sprintf(buf, name);
    *buf++ = ' ';
    buf = moro8_print_word(buf, value);
    *buf++ = '\n';
    return buf;
}

/** Print REG: LL HH\n */
static inline char* moro8_print_register_dword(char* buf, const char* name, moro8_udword value)
{
    buf += sprintf(buf, name);
    *buf++ = ' ';
    buf = moro8_print_dword(buf, value);
    *buf++ = '\n';
    return buf;
}

/** Print the whole memory as 0000: 00 00 00 00 ... 00 00 00 00 */
static inline char* moro8_print_memory(char* buf, const moro8_vm* vm)
{
    moro8_udword address = 0;
    for (moro8_udword i = 0; i < MORO8_MEMORY_SIZE - 0x10; i += 0x10)
    {
        if (i > 0)
        {
            *buf++ = '\n';
        }

        buf = moro8_print_word(buf, (moro8_uword)MORO8_HIGH(i));
        buf = moro8_print_word(buf, (moro8_uword)MORO8_LOW(i));
        *buf++ = ':';
        *buf++ = ' ';

        for (moro8_uword j = 0; j <= 0xF; ++j)
        {
            if (j > 0)
            {
                *buf++ = ' ';
                if ((j % 4) == 0)
                {
                    *buf++ = ' ';
                }
            }
            buf = moro8_print_word(buf, MORO8_GET_MEM(address++));
        }
    }

    return buf;
}

char* moro8_print(const moro8_vm* vm)
{
    char* buf = (char*)malloc(MORO8_PRINT_BUFFER_SIZE + 1);
    if (!buf)
    {
        return NULL;
    }

    char* ptr = buf;
    ptr = moro8_print_register_dword(ptr, "PC:", vm->registers.pc);
    ptr = moro8_print_register_word(ptr, "AC:", vm->registers.ac);
    ptr = moro8_print_register_word(ptr, "X: ", vm->registers.x);
    ptr = moro8_print_register_word(ptr, "Y: ", vm->registers.y);
    ptr = moro8_print_register_word(ptr, "SP:", vm->registers.sp);
    ptr = moro8_print_register_word(ptr, "N: ", vm->registers.sr.n);
    ptr = moro8_print_register_word(ptr, "V: ", vm->registers.sr.v);
    ptr = moro8_print_register_word(ptr, "Z: ", vm->registers.sr.z);
    ptr = moro8_print_register_word(ptr, "C: ", vm->registers.sr.c);
    *ptr++ = '\n';
    ptr = moro8_print_memory(ptr, vm);
    ptr[0] = '\0';

    return buf;
}

/** Parse a 0-9A-F character */
static inline moro8_uword moro8_parse_hex(char value)
{
    return (value >= '0' && value <= '9') ? (value - '0') : (value - 'A' + 10);
}

#define MORO8_IS_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
/** Parse a HHLL hex string to double word */
static inline int moro8_parse_address(const char* buf, moro8_udword* value)
{
    if (!(MORO8_IS_HEX(buf[0]) && MORO8_IS_HEX(buf[1]) && MORO8_IS_HEX(buf[2]) && MORO8_IS_HEX(buf[3])))
    {
        return MORO8_FALSE;
    }

    *value = (((moro8_parse_hex(buf[0]) << 4) + moro8_parse_hex(buf[1])) << 8) + ((moro8_parse_hex(buf[2]) << 4) + moro8_parse_hex(buf[3]));
    return MORO8_TRUE;
}

moro8_vm* moro8_parse(moro8_vm* vm, const char* buf, size_t size)
{
#define MORO8_IS_LETTER_OR_HEX(c) ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z'))
#define MORO8_STATE_IDLE 0
#define MORO8_STATE_VALUE 1
#define MORO8_STATE_COMMENT 2

    moro8_reset(vm);
    moro8_uword* value_buffer = NULL;
    int state = MORO8_STATE_IDLE;
    char c = 0;
    size_t line = 1;
    size_t col = 1;
    size_t value_index = 0;
    moro8_udword address = 0;

    for (size_t i = 0; i < size;)
    {
        c = *buf;

        // Consume whitespaces and newlines
        switch (c)
        {
        case '\0':
            return vm;
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
                    if (j == 4 && moro8_parse_address(buf, &address))
                    {
                        // Found an address
                        // TODO value_buffer = &vm->state.memory[address];
                    }
                    else if (buf[0] == 'P' && buf[1] == 'C')
                    {
                        vm->registers.pc = 0;
                        value_buffer = (moro8_uword*)&vm->registers.pc;
                    }
                    else if (buf[0] == 'A' && buf[1] == 'C')
                    {
                        value_buffer = &vm->registers.ac;
                    }
                    else if (buf[0] == 'X')
                    {
                        value_buffer = &vm->registers.x;
                    }
                    else if (buf[0] == 'Y')
                    {
                        value_buffer = &vm->registers.y;
                    }
                    else if (buf[0] == 'S' && buf[1] == 'P')
                    {
                        value_buffer = &vm->registers.sp;
                    }
                    else if (buf[0] == 'N')
                    {
                        value_buffer = &vm->registers.sr.n;
                    }
                    else if (buf[0] == 'V')
                    {
                        value_buffer = &vm->registers.sr.v;
                    }
                    else if (buf[0] == 'C')
                    {
                        value_buffer = &vm->registers.sr.c;
                    }
                    else if (buf[0] == 'Z')
                    {
                        value_buffer = &vm->registers.sr.z;
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

            value_buffer[value_index / 2] += moro8_parse_hex(c) << (((value_index + 1) % 2) * 4);

            ++value_index;
            ++buf;
            ++col;
        }
    }

    return vm;

#undef MORO8_IS_HEX
#undef MORO8_IS_LETTER_OR_HEX
#undef MORO8_STATE_IDLE
#undef MORO8_STATE_VALUE
#undef MORO8_STATE_COMMENT
#undef MORO8_IS_ADDRESS
}

#endif /* !MORO8_WITH_PARSER */

#if MORO8_WITH_HOOKS

moro8_hook moro8_get_hook(moro8_vm* vm, moro8_uword op)
{
    return vm->hooks[op];
}

void moro8_set_hook(moro8_vm* vm, moro8_uword op, moro8_hook hook)
{
    vm->hooks[op] = hook;
}

#endif