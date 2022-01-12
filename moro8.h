#ifndef MORO8__h
#define MORO8__h

#ifdef __cplusplus
extern "C"
{
#endif

/** Major version of moro8. */
#define MORO8_VERSION_MAJOR 0
/** Minor version of moro8. */
#define MORO8_VERSION_MINOR 1
/** Patch version of moro8. */
#define MORO8_VERSION_PATCH 0

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef HAVE_STDIO_H
#define HAVE_STDIO_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

/* Define to 1 if you have the `free' function. */
#ifndef HAVE_FREE
#define HAVE_FREE 1
#endif

/* Define to 1 if you have the `malloc' function. */
#ifndef HAVE_MALLOC
#define HAVE_MALLOC 1
#endif

/* Define to 1 if you build with Doxygen. */
#ifndef MORO8_DOXYGEN
#define MORO8_DOXYGEN 1
#endif

#ifndef MORO8_MALLOC
#ifdef HAVE_MALLOC
/**
 * Defines the malloc function used by moro8 at compile time.
 *
 * @code
 * void* my_malloc(size_t size)
 * {
 *     // do something
 * }
 * 
 * #define MORO8_MALLOC my_malloc
 * @endcode
 */
#define MORO8_MALLOC malloc
#else
#define MORO8_MALLOC(size) NULL
#endif
#endif

#ifndef MORO8_FREE
#ifdef HAVE_FREE
/**
* Defines the free function used by moro8 at compile time.
*
* @code 
* void my_free(void* ptr)
* {
*     // do something
* }
* 
* #define MORO8_FREE my_free
* @endcode
*/
#define MORO8_FREE free
#else
#define MORO8_FREE(ptr)
#endif
#endif

#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
#define MORO8_CDECL __cdecl
#define MORO8_STDCALL __stdcall

/* export symbols by default, this is necessary for copy pasting the C and header file */
#if !defined(MORO8_HIDE_SYMBOLS) && !defined(MORO8_IMPORT_SYMBOLS) && !defined(MORO8_EXPORT_SYMBOLS)
#define MORO8_EXPORT_SYMBOLS
#endif

#if defined(MORO8_HIDE_SYMBOLS)
#define MORO8_PUBLIC(type)   type MORO8_STDCALL
#elif defined(MORO8_EXPORT_SYMBOLS)
#define MORO8_PUBLIC(type)   __declspec(dllexport) type MORO8_STDCALL
#elif defined(MORO8_IMPORT_SYMBOLS)
#define MORO8_PUBLIC(type)   __declspec(dllimport) type MORO8_STDCALL
#endif
#else /* !__WINDOWS__ */
#define MORO8_CDECL
#define MORO8_STDCALL

#if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
#define MORO8_PUBLIC(type)   __attribute__((visibility("default"))) type
#else
#define MORO8_PUBLIC(type) type
#endif
#endif /* __WINDOWS__ */

/** Type for an unsigned single byte. */
typedef unsigned char moro8_uword;
/** Type for an unsigned double byte. */
typedef unsigned short moro8_udword;
/** Type for a signed single byte. */
typedef char moro8_word;
/** Type for a signed double byte. */
typedef short moro8_dword;

// Macros for the memory structure

/** Total size of the memory */
#define MORO8_MEMORY_SIZE 0xFFFF
/** Offset of the zero zp in memory */
#define MORO8_ZP_OFFSET 0
/** Total size of the zp area */
#define MORO8_ZP_SIZE 0xFF
/** Offset of the progmem area in memory */
#define MORO8_PROGMEM_OFFSET MORO8_ZP_SIZE
/** Total size of the progmem area */
#define MORO8_PROGMEM_SIZE 0xFF
/** Offset of the RAM area in memory */
#define MORO8_RAM_OFFSET (MORO8_PROGMEM_OFFSET + MORO8_PROGMEM_SIZE)
/** Total size of the RAM area */
#define MORO8_RAM_SIZE (MORO8_MEMORY_SIZE - MORO8_RAM_OFFSET)

/** Gets the high part of a 16 bit (double word) address */
#define MORO8_HIGH(address) ((address & 0xFF00) >> 8)
/** Gets the low part of a 16 bit (double word) address */
#define MORO8_LOW(address) (address & 0xFF)
/** Get the address at ZP + offset */
#define MORO8_ZP(offset) (MORO8_ZP_OFFSET + offset)
/** Get the high byte of address at ZP + offset */
#define MORO8_HZP(offset) ((MORO8_ZP(offset) & 0xFF00) >> 8)
/** Get the low byte of address at ZP + offset */
#define MORO8_LZP(offset) (MORO8_ZP(offset) & 0xFF)
/** Get the address at PROGMEM + offset */
#define MORO8_PROGMEM(offset) (MORO8_PROGMEM_OFFSET + offset)
/** Get the high byte of address at PROGMEM + offset */
#define MORO8_HPROGMEM(offset) ((MORO8_PROGMEM(offset) & 0xFF00) >> 8)
/** Get the low byte of address at PROGMEM + offset */
#define MORO8_LPROGMEM(offset) (MORO8_PROGMEM(offset) & 0xFF)
/** Get the address at RAM + offset */
#define MORO8_RAM(offset) (MORO8_RAM_OFFSET + offset)
/** Get the high byte of address at RAM + offset */
#define MORO8_HRAM(offset) ((MORO8_RAM(offset) & 0xFF00) >> 8)
/** Get the low byte of address at RAM + offset */
#define MORO8_LRAM(offset) (MORO8_RAM(offset) & 0xFF)

/**
 * The required size for a buffer passed to moro8_print not
 * including the null-terminating character.
 * 
 * Fortunately, the size of our vm and the number of required
 * bytes for printing its state are well known at compile time.
 */
#define MORO8_PRINT_BUFFER_SIZE 253485

enum moro8_opcode
{
	MORO8_OP_ADC_IMM = 0x69,
	MORO8_OP_ADC_ZP = 0x65,
	MORO8_OP_ADC_ZP_X = 0x75,
	MORO8_OP_ADC_ABS = 0x6D,
	MORO8_OP_ADC_ABS_X = 0x7D,
	MORO8_OP_ADC_ABS_Y = 0x79,
	MORO8_OP_ADC_IND_X = 0x61,
	MORO8_OP_ADC_IND_Y = 0x71,
	MORO8_OP_AND_IMM = 0x29,
	MORO8_OP_AND_ZP = 0x25,
	MORO8_OP_AND_ZP_X = 0x35,
	MORO8_OP_AND_ABS = 0x2D,
	MORO8_OP_AND_ABS_X = 0x3D,
	MORO8_OP_AND_ABS_Y = 0x39,
	MORO8_OP_AND_IND_X = 0x21,
	MORO8_OP_AND_IND_Y = 0x31,
	MORO8_OP_ASL_AC = 0x0A,
	MORO8_OP_ASL_ZP = 0x06,
	MORO8_OP_ASL_ZP_X = 0x16,
	MORO8_OP_ASL_ABS = 0x0E,
	MORO8_OP_ASL_ABS_X = 0x1E,
	MORO8_OP_BCC = 0x90,
	MORO8_OP_BCS = 0xB0,
	MORO8_OP_BEQ = 0xF0,
	MORO8_OP_BMI = 0x30,
	MORO8_OP_BNE = 0xD0,
	MORO8_OP_BPL = 0x10,
	MORO8_OP_BVC = 0x50,
	MORO8_OP_BVS = 0x70,
	MORO8_OP_CLC = 0x18,
	MORO8_OP_CLV = 0xB8,
	MORO8_OP_EOR_IMM = 0x49,
	MORO8_OP_EOR_ZP = 0x45,
	MORO8_OP_EOR_ZP_X = 0x55,
	MORO8_OP_EOR_ABS = 0x4D,
	MORO8_OP_EOR_ABS_X = 0x5D,
	MORO8_OP_EOR_ABS_Y = 0x59,
	MORO8_OP_EOR_IND_X = 0x41,
	MORO8_OP_EOR_IND_Y = 0x51,
	MORO8_OP_JMP_ABS = 0x4C,
	MORO8_OP_JMP_IND = 0x6C,
	MORO8_OP_JSR_ABS = 0x20,
	MORO8_OP_LDA_IMM = 0xA9,
	MORO8_OP_LDA_ZP = 0xA5,
	MORO8_OP_LDA_ZP_X = 0xB5,
	MORO8_OP_LDA_ABS = 0xAD,
	MORO8_OP_LDA_ABS_X = 0xBD,
	MORO8_OP_LDA_ABS_Y = 0xB9,
	MORO8_OP_LDA_IND_X = 0xA1,
	MORO8_OP_LDA_IND_Y = 0xB1,
	MORO8_OP_LDX_IMM = 0xA2,
	MORO8_OP_LDX_ZP = 0xA6,
	MORO8_OP_LDX_ZP_Y = 0xB6,
	MORO8_OP_LDX_ABS = 0xAE,
	MORO8_OP_LDX_ABS_Y = 0xBE,
	MORO8_OP_LDY_IMM = 0xA0,
	MORO8_OP_LDY_ZP = 0xA4,
	MORO8_OP_LDY_ZP_X = 0xB4,
	MORO8_OP_LDY_ABS = 0xAC,
	MORO8_OP_LDY_ABS_X = 0xBC,
	MORO8_OP_LSR_AC = 0x4A,
	MORO8_OP_LSR_ZP = 0x46,
	MORO8_OP_LSR_ZP_X = 0x56,
	MORO8_OP_LSR_ABS = 0x4E,
	MORO8_OP_LSR_ABS_X = 0x5E,
	MORO8_OP_NOP = 0xEA,
	MORO8_OP_ORA_IMM = 0x09,
	MORO8_OP_ORA_ZP = 0x05,
	MORO8_OP_ORA_ZP_X = 0x15,
	MORO8_OP_ORA_ABS = 0x0D,
	MORO8_OP_ORA_ABS_X = 0x1D,
	MORO8_OP_ORA_ABS_Y = 0x19,
	MORO8_OP_ORA_IND_X = 0x01,
	MORO8_OP_ORA_IND_Y = 0x11,
	MORO8_OP_PHA = 0x48,
	MORO8_OP_PLA = 0x68,
	MORO8_OP_STA_ZP = 0x85,
	MORO8_OP_STA_ZP_X = 0x95,
	MORO8_OP_STA_ABS = 0x8D,
	MORO8_OP_STA_ABS_X = 0x9D,
	MORO8_OP_STA_ABS_Y = 0x99,
	MORO8_OP_STA_IND_X = 0x81,
	MORO8_OP_STA_IND_Y = 0x91,
	MORO8_OP_STX_ZP = 0x86,
	MORO8_OP_STX_ZP_Y = 0x96,
	MORO8_OP_STX_ABS = 0x8E,
	MORO8_OP_STY_ZP = 0x84,
	MORO8_OP_STY_ZP_X = 0x94,
	MORO8_OP_STY_ABS = 0x8C,
	MORO8_OP_TAX = 0xAA,
	MORO8_OP_TAY = 0xA8,
	MORO8_OP_TSX = 0xBA,
	MORO8_OP_TXA = 0x8A,
	MORO8_OP_TXS = 0x9A,
	MORO8_OP_TYA = 0x98,
	// Number of opcodes
	MORO8_OP_MAX
};

enum moro8_register
{
	// Accumulator
	MORO8_REGISTER_AC,
	// Index registers
	MORO8_REGISTER_X,
	MORO8_REGISTER_Y,
	// Status register
	MORO8_REGISTER_SR,
	// Stack pointer
	MORO8_REGISTER_SP,
	// Number of registers
	MORO8_REGISTER_MAX
};

struct moro8_vm;

/**
 * Allocates and returns a pointer to a new vm instance.
 * @code
 * struct moro8_vm* vm = moro8_create();
 * 
 * ...
 * 
 * // Don't forget to free memory
 * moro8_delete(vm);
 * @endcode
 * @return Pointer to new vm instance.
 */
MORO8_PUBLIC(struct moro8_vm*) moro8_create();

/**
 * Resets all registers and memory of a vm to initial state.
 * @param[in] vm Some vm instance
 */
MORO8_PUBLIC(void) moro8_reset(struct moro8_vm* vm);

/**
 * Frees up memory allocated for a vm instance.
 * @code
 * struct moro8_vm* vm = moro8_create();
 * 
 * ...
 * 
 * // Don't forget to free memory
 * moro8_delete(vm);
 * @endcode
 * @param[in] vm Some vm instance
 */
MORO8_PUBLIC(void) moro8_delete(struct moro8_vm* vm);

/**
 * Gets the underlying memory buffer for raw manipulations.
 * 
 * Note that the returned buffer is const. Indeed there is no
 * need to directly modify this buffer when the API already
 * provides all the required functions to do so. The use
 * case for moro8_as_buffer is more for when you want to save
 * or load the vm on disk.
 * @code
 * // Get the underlying memory buffer and size
 * size_t size = 0;
 * void* buf = moro8_as_buffer(vm, &size);
 * 
 * // Write bytes to file
 * FILE* f = fopen(filename, "w");
 * fwrite(buf, size, 1, f);
 * fclose(f);
 * 
 * // Don't access buf after deleting the vm
 * moro8_delete(vm);
 * @endcode
 * @param[in] vm Some vm instance
 * @param[out] size Buffer size
 * @return A pointer to the memory buffer used by the vm.
 */
MORO8_PUBLIC(const void*) moro8_as_buffer(const struct moro8_vm* vm, size_t* size);

/**
 * Initializes a vm from a memory buffer.
 * 
 * Note that the buffer is copied to the vm. This means
 * that you can freely modify or delete the buffer after
 * calling moro8_from_buffer.
 * @code
 * // Fetch file size
 * FILE* f = fopen(filename, "r");
 * fseek(f, 0, SEEK_END);
 * size_t size = ftell(f);
 * fseek(f, 0, SEEK_SET);
 * 
 * // Read bytes from file
 * void* buf = malloc(size);
 * fread(buf, size, 1, f);
 * fclose(f);
 * 
 * // Initialize vm from memory buffer and size
 * moro8_from_buffer(vm, buf, size);
 * 
 * free(buf);
 * @endcode
 * @param[in] vm Some vm instance
 * @param[in] buf Some memory buffer
 * @param[in] size Buffer size
 */
MORO8_PUBLIC(void) moro8_from_buffer(struct moro8_vm* vm, const void* buf, size_t size);

/**
 * Creates a snapshot from a vm.
 * @code
 * // Execute next instruction
 * moro8_step(vm);
 * 
 * // Backup the current state
 * moro8_vm* snapshot = moro8_create();
 * moro8_backup(snapshot, vm);
 * 
 * // Execute next instruction
 * moro8_step(vm);
 * 
 * // Restore previous state
 * moro8_restore(vm, snapshot);
 * moro8_delete(snapshot);
 * @endcode
 * @param[out] snapshot VM to copy to
 * @param[in] vm VM to copy from
 */
MORO8_PUBLIC(void) moro8_backup(struct moro8_vm* snapshot, const struct moro8_vm* vm);

/**
 * Restores a snapshot to a vm.
 * @code
 * // Execute next instruction
 * moro8_step(vm);
 * 
 * // Backup the current state
 * moro8_vm* snapshot = moro8_create();
 * moro8_backup(snapshot, vm);
 * 
 * // Execute next instruction
 * moro8_step(vm);
 * 
 * // Restore previous state
 * moro8_restore(vm, snapshot);
 * moro8_delete(snapshot);
 * @endcode
 * @param[out] vm VM to copy to
 * @param[in] snapshot VM to copy from
 */
MORO8_PUBLIC(void) moro8_restore(struct moro8_vm* vm, const struct moro8_vm* snapshot);

/**
 * Returns true only if the two vm are in the exact same state.
 * @param[in] left Some vm
 * @param[in] right Another vm
 * @return If the two vm are the same.
 */
MORO8_PUBLIC(int) moro8_equal(const struct moro8_vm* left, const struct moro8_vm* right);

/**
 * Dumps the current vm state to an human-readable string.
 * @param[in] vm Some vm
 * @return String representation of the vm state.
 */
MORO8_PUBLIC(char*) moro8_print(const struct moro8_vm* vm);

/**
 * Parses the string representation of a vm state.
 * @param[out] vm Some vm
 * @param[in] buf Some buffer
 * @param[in] size Buffer size
 * @return The same vm pointer if there is no errors, NULL otherwise.
 */
MORO8_PUBLIC(struct moro8_vm*) moro8_parse(struct moro8_vm* vm, const char* buf, size_t size);

/**
 * Loads a program to memory.
 * @param vm vm instance
 * @param prog program address
 * @param size program length
 */
MORO8_PUBLIC(void) moro8_load(struct moro8_vm* vm, const moro8_uword* prog, moro8_udword size);

MORO8_PUBLIC(size_t) moro8_step(struct moro8_vm* vm);

MORO8_PUBLIC(void) moro8_run(struct moro8_vm* vm);

/**
 * Gets the address stored in program counter register.
 * @param[in] vm Some vm
 * @return Value in program counter.
 */
MORO8_PUBLIC(moro8_udword) moro8_get_pc(const struct moro8_vm* vm);

/**
 * Sets the address stored in program counter register.
 * @param[in] vm Some vm
 * @param[in] address Absolute address
 */
MORO8_PUBLIC(void) moro8_set_pc(struct moro8_vm* vm, moro8_udword address);

MORO8_PUBLIC(moro8_uword) moro8_get_register(const struct moro8_vm* vm, enum moro8_register reg);

MORO8_PUBLIC(void) moro8_set_register(struct moro8_vm* vm, enum moro8_register reg, moro8_uword value);

MORO8_PUBLIC(moro8_udword) moro8_get_memory(const struct moro8_vm* vm, moro8_uword* buffer, moro8_udword offset, moro8_udword size);

MORO8_PUBLIC(moro8_udword) moro8_set_memory(struct moro8_vm* vm, const moro8_uword* buffer, moro8_udword offset, moro8_udword size);

MORO8_PUBLIC(int) moro8_load_state(struct moro8_vm* vm, const char* path);

MORO8_PUBLIC(int) moro8_dump_state(struct moro8_vm* vm, const char* path);

/** Get/Set a single word in memory */
#define moro8_get_memory_word(vm, offset, value) moro8_get_memory(vm, &value, offset, 1)
#define moro8_set_memory_word(vm, offset, value) { const moro8_uword __buffer[1] = {value}; moro8_set_memory(vm, __buffer, offset, 1); }
/** Set a double word in memory */
#define moro8_set_memory_dword(vm, offset, value) { const moro8_uword __buffer[2] = {(value & 0xFF00) >> 8, value & 0xFF}; moro8_set_memory(vm, __buffer, offset, 2); }
/** Set memory starting at ZP + offset */
#define moro8_set_zp(vm, buffer, offset, size) moro8_set_memory(vm, buffer, MORO8_ZP(offset), size)
/** Get/Set a single word at ZP + offset */
#define moro8_get_zp_word(vm, offset, value) moro8_get_memory_word(vm, MORO8_ZP(offset), value)
#define moro8_set_zp_word(vm, offset, value) moro8_set_memory_word(vm, MORO8_ZP(offset), value)
/** Set a double word at ZP + offset */
#define moro8_set_zp_dword(vm, offset, value) moro8_set_memory_dword(vm, MORO8_ZP(offset), value)
/** Set memory starting at PROGMEM + offset */
#define moro8_set_progmem(vm, buffer, offset, size) moro8_set_memory(vm, buffer, MORO8_PROGMEM(offset), size)
/** Get/Set a single word at PROGMEM + offset */
#define moro8_get_progmem_word(vm, offset, value) moro8_get_memory_word(vm, MORO8_PROGMEM(offset), value)
#define moro8_set_progmem_word(vm, offset, value) moro8_set_memory_word(vm, MORO8_PROGMEM(offset), value)
/** Set a double word at PROGMEM + offset */
#define moro8_set_progmem_dword(vm, offset, value) moro8_set_memory_dword(vm, MORO8_PROGMEM(offset), value)
/** Set memory starting at RAM + offset */
#define moro8_set_ram(vm, buffer, offset, size) moro8_set_memory(vm, buffer, MORO8_RAM(offset), size)
/** Get/Set a single word at RAM + offset */
#define moro8_get_ram_word(vm, offset, value) moro8_get_memory_word(vm, MORO8_RAM(offset), value)
#define moro8_set_ram_word(vm, offset, value) moro8_set_memory_word(vm, MORO8_RAM(offset), value)
/** Set a double word at RAM + offset */
#define moro8_set_ram_dword(vm, offset, value) moro8_set_memory_dword(vm, MORO8_RAM(offset), value)
/** Get registers values */
#define moro8_get_a(vm) moro8_get_register(vm, MORO8_REGISTER_A)
#define moro8_get_x(vm) moro8_get_register(vm, MORO8_REGISTER_X)
#define moro8_get_y(vm) moro8_get_register(vm, MORO8_REGISTER_Y)
#define moro8_get_s(vm) moro8_get_register(vm, MORO8_REGISTER_S)
/** Set registers values */
#define moro8_set_a(vm, value) moro8_set_register(vm, MORO8_REGISTER_A, value)
#define moro8_set_x(vm, value) moro8_set_register(vm, MORO8_REGISTER_X, value)
#define moro8_set_y(vm, value) moro8_set_register(vm, MORO8_REGISTER_Y, value)
#define moro8_set_s(vm, value) moro8_set_register(vm, MORO8_REGISTER_S, value)

#ifdef __cplusplus
}
#endif

#endif
