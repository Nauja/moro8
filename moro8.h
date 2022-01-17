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
#endif

#if defined(MORO8_DOXYGEN)
#undef MORO8_MINIMALIST
/**
* Strips extra features from moro8.
*
* This implicitly sets MORO8_WITH_PARSER=0 and MORO8_WITH_HOOKS=0.
*/
#define MORO8_MINIMALIST 0
#endif

#if !MORO8_MINIMALIST && !defined(MORO8_WITH_PARSER)
/**
* Builds moro8 with support for parser.
*
* @note
* Parser is not available when MORO8_MINIMALIST is defined.
* 
*/
#define MORO8_WITH_PARSER 1
#endif

#if !MORO8_MINIMALIST && !defined(MORO8_WITH_HOOKS)
/**
* Builds moro8 with support for hooks.
*
* A hook is a function that can be registered to the vm for
* custom handling of opcodes.
*
* @note
* Hooks are not available when MORO8_MINIMALIST is defined.
*
*/
#define MORO8_WITH_HOOKS 1
#endif

/** Type for an unsigned single byte (8-bit). */
typedef unsigned char moro8_uword;
/** Type for an unsigned double byte (16-bit). */
typedef unsigned short moro8_udword;
/** Type for a signed single byte (8-bit). */
typedef char moro8_word;
/** Type for a signed double byte (16-bit). */
typedef short moro8_dword;

/**
 * Total size of the memory allocated for the vm: 0x10000.
 * 
 * With a double word (16-bit) address, we can address up to 65536 bytes.
 */
#define MORO8_MEMORY_SIZE 0x10000
 /**
  * Total size of a single page in memory: 0x100.
  *
  * With a single word (8-bit) address, we can address up to 256 bytes.
  */
#define MORO8_PAGE_SIZE 0x100
 /**
  * Starting address of the stack in memory: 0x100.
  * 
  * The stack starts after the zero page, thus at address 0x100.
  */
#define MORO8_STACK_OFFSET 0x100
 /**
  * Total size of the stack in memory: 0x100.
  *
  * As the stack register is a single word (8-bit), we can address up to 256 bytes.
  */
#define MORO8_STACK_SIZE 0x100
/**
 * Total size of the ROM section in memory.
 *
 * This is an arbitrary value but must be big enough to contain the program.
 */
#define MORO8_ROM_SIZE 0xFF
/**
 * Starting address of the ROM section in memory: MORO8_MEMORY_SIZE - MORO8_ROM_SIZE.
 * 
 * The ROM section is always at the high end of memory.
 */
#define MORO8_ROM_OFFSET (MORO8_MEMORY_SIZE - MORO8_ROM_SIZE)
/**
 * Offset of the RAM section in memory: 0x200.
 *
 * The RAM is always at the low end of memory, right after the stack.
 */
#define MORO8_RAM_OFFSET 0x200
/**
 * Total size of the RAM section in memory: MORO8_ROM_OFFSET - MORO8_RAM_OFFSET.
 *
 * This is all memory that is no reserved by the CPU and is not ROM.
 */
#define MORO8_RAM_SIZE (MORO8_ROM_OFFSET - MORO8_RAM_OFFSET)
/**
 * Address of the RES (Reset) Vector in memory: 0xFFFC.
 *
 * This points to the first address of the program in memory.
 */
#define MORO8_RES_VECTOR_ADDRESS 0xFFFC
/** Address of the IRQ (Interrupt Request) Vector in memory: 0xFFFE. */
#define MORO8_IRQ_VECTOR_ADDRESS 0xFFFE

/** Gets the high part of a 16 bit (double word) address */
#define MORO8_HIGH(address) ((address & 0xFF00) >> 8)
/** Gets the low part of a 16 bit (double word) address */
#define MORO8_LOW(address) (address & 0xFF)
/** Get the address at ZP + offset */
#define MORO8_ZP(offset) offset
/** Get the high byte of address at ZP + offset */
#define MORO8_HZP(offset) ((MORO8_ZP(offset) & 0xFF00) >> 8)
/** Get the low byte of address at ZP + offset */
#define MORO8_LZP(offset) (MORO8_ZP(offset) & 0xFF)
/** Get the address at PROGMEM + offset */
#define MORO8_PROGMEM(offset) (MORO8_ROM_OFFSET + offset)
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

struct moro8_vm;

#if MORO8_WITH_HOOKS

/**
 * Signature of a hook function for handling opcodes.
 * 
 * @note
 * Available only if built with MORO8_WITH_HOOKS=1
 * 
 * @param[in] vm VM instance
 * @param[in] op Opcode to handle
 * @return If the instruction was handled.
 */
typedef int(*moro8_hook)(struct moro8_vm*, moro8_uword op);

/**
 * Gets the registered hook function for a specific opcode.
 *
 * @note
 * Available only if built with MORO8_WITH_HOOKS=1
 * 
 * @param[in] vm Some vm
 * @param[in] op Opcode number
 * @return Registered hook function or NULL.
 */
MORO8_PUBLIC(moro8_hook) moro8_get_hook(struct moro8_vm* vm, moro8_uword op);

/**
 * Registers a hook for handling a specific opcode.
 * 
 * Example of registering a hook function for printing the JMP instructions:
 * @code
 * int print_jmp(struct moro8_vm* vm, moro8_uword op) {
 *     printf("JMP");
 * 
 *     // Let the vm handle this instruction
 *     return 0;
 * }
 *
 * moro8_set_hook(vm, MORO8_OP_JMP_ABS, print_jmp);
 * @endcode
 *
 * Beware that it will replace already registered hook if any.
 *
 * @note
 * Available only if built with MORO8_WITH_HOOKS=1
 * 
 * @param[in] vm Some vm
 * @param[in] op Opcode number
 * @param[in] hook Pointer to hook
 */
MORO8_PUBLIC(void) moro8_set_hook(struct moro8_vm* vm, moro8_uword op, moro8_hook hook);

#endif

/** Struct containing registers used by the CPU. */
struct moro8_registers {
    /** Program counter. */
    moro8_udword pc;
    /** Accumulator: math register. */
    moro8_uword ac;
    /** X: index registers. */
    moro8_uword x;
    /** Y: index registers. */
    moro8_uword y;
    /** Status register. */
    struct sr
    {
        /** Negative. */
        moro8_uword n;
        /** Overflow. */
        moro8_uword v;
        /** Zero. */
        moro8_uword z;
        /** Carry. */
        moro8_uword c;
    } sr;
    /** Stack pointer. */
    moro8_uword sp;
};

/**
 * Struct representing a bus allowing the CPU to read and write memory.
 *
 * You can imagine it as a physical bus connected to the CPU and the memory,
 * allowing the CPU to read and write memory.
 */
struct moro8_bus {
	/**
	 * Gets the memory at a specific address.
	 * @param[in] bus Pointer to the bus instance
	 * @param[in] address Absolute address
	 * @return Memory at requested address.
	 */
	moro8_uword (*get)(const struct moro8_bus* bus, moro8_udword address);

	/**
	 * Sets the memory at a specific address.
	 * @param[in] bus Pointer to the bus instance
	 * @param[in] address Absolute address
	 * @param[in] value New value
	 */
	void (*set)(struct moro8_bus* bus, moro8_udword address, moro8_uword value);
};

/**
 * Memory implementation using a continuous array of MORO8_MEMORY_SIZE bytes.
 *
 * This is suitable for running on systems with more than MORO8_MEMORY_SIZE
 * available RAM.
 */
struct moro8_array_memory {
	/**
	 * The bus connected to this memory.
	 *
	 * You have to connect it to the CPU for it to work.
	 */
	struct moro8_bus bus;
	/**
	 * A continuous array of MORO8_MEMORY_SIZE bytes allowing for fast read
	 * and write operations.
	 */
	moro8_udword buffer[MORO8_MEMORY_SIZE];
};

/** Struct for the vm emulating the moro8 CPU. */
struct moro8_vm {
	/** Registers used by the CPU. */
	struct moro8_registers registers;
	/** Bus connecting the CPU to memory. */
	struct moro8_bus* memory;
#if MORO8_WITH_HOOKS
	/**
	 * Registered hooks for custom handling of opcodes.
     * @note
     * Available only if built with MORO8_WITH_HOOKS=1
	 */
	moro8_hook hooks[0x100];
#endif
};

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
	/** Accumulator register. */
	MORO8_REGISTER_AC,
	/** Index register. */
	MORO8_REGISTER_X,
	/** Index register. */
	MORO8_REGISTER_Y,
	/** Status register. */
	MORO8_REGISTER_SR,
	/** Negative register. */
	MORO8_REGISTER_N,
	/** Overflow register. */
	MORO8_REGISTER_V,
	/** Zero register. */
	MORO8_REGISTER_Z,
	/** Carry register. */
	MORO8_REGISTER_C,
	/** Stack pointer. */
	MORO8_REGISTER_SP
};

/**
 * Allocates and initializes a new moro8_array_memory instance.
 * @code
 * struct moro8_array_memory* memory = moro8_array_memory_create();
 * 
 * ...
 * 
 * // Don't forget to free up memory
 * moro8_array_memory_delete(memory);
 * @endcode
 * @return Pointer to new moro8_array_memory instance.
 */
MORO8_PUBLIC(struct moro8_array_memory*) moro8_array_memory_create();

/**
 * Initializes an already allocated moro8_array_memory instance.
 * @code
 * struct moro8_array_memory memory;
 * moro8_array_memory_init(&memory);
 * @endcode
 *
 * This initializes the bus correctly with:
 * @code 
 * memory.bus.get = &moro8_array_memory_get;
 * memory.bus.set = &moro8_array_memory_set;
 * @endcode
 * @param[in] memory Some moro8_array_memory instance
 */
MORO8_PUBLIC(void) moro8_array_memory_init(struct moro8_array_memory* memory);

/**
 * Frees up memory allocated for a moro8_array_memory instance.
 * @code
 * struct moro8_array_memory* memory = moro8_array_memory_create();
 * 
 * ...
 * 
 * // Don't forget to free up memory
 * moro8_array_memory_delete(memory);
 * @endcode
 * @param[in] memory Some moro8_array_memory instance
 */
MORO8_PUBLIC(void) moro8_array_memory_delete(struct moro8_array_memory* vm);

/**
 * Gets memory from an instance of moro8_array_memory.
 * @code
 * struct moro8_array_memory memory;
 * moro8_array_memory_init(&memory);
 *
 * moro8_uword value = moro8_array_memory_get(&memory, 0xFF);
 * @endcode
 * @param[in] memory Some moro8_array_memory instance
 * @param[in] address Absolute address
 * @return Memory at requested address.
 */
MORO8_PUBLIC(moro8_uword) moro8_array_memory_get(const struct moro8_array_memory* memory, moro8_udword address);

/**
 * Sets memory to an instance of moro8_array_memory.
 * @code
 * struct moro8_array_memory memory;
 * moro8_array_memory_init(&memory);
 *
 * moro8_array_memory_set(&memory, 0xFF, 0x1);
 * @endcode
 * @param[in] memory Some moro8_array_memory instance
 * @param[in] address Absolute address
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_array_memory_set(struct moro8_array_memory* memory, moro8_udword address, moro8_uword value);

/**
 * Allocates and initializes a new moro8_vm instance.
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
 * Initializes an already allocated moro8_vm instance.
 *
 * This is called implicitly by moro8_create, but must be called
 * manually otherwise.
 * @code
 * struct moro8_vm vm;
 * moro8_init(&vm);
 * 
 * ...
 * @endcode
 * @param[in] vm Some vm instance
 */
MORO8_PUBLIC(void) moro8_init(struct moro8_vm* vm);

/**
 * Frees up memory allocated for a moro8_vm instance.
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
 * Assigns the bus for connecting the CPU to memory.
 * @code
 * struct moro8_vm* vm = moro8_create();
 * struct moro8_array_memory* memory = moro8_array_memory_create();
 *
 * moro8_connect_memory(vm, &memory->bus);
 *
 * // Disconnect
 * moro8_connect_memory(vm, NULL);
 * @endcode
 * @param[in] vm Some vm instance
 */
MORO8_PUBLIC(void) moro8_connect_memory(struct moro8_vm* vm, struct moro8_bus* bus);

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

/**
 * Gets size bytes from memory starting at offset.
 *
 * @code
 * moro8_uword buffer[4];
 * 
 * moro8_get_memory(vm, buffer, 0x1000, 4);
 * @endcode
 * @param[in] vm Some vm
 * @param[in] buffer Pointer to buffer for storing the bytes
 * @param[in] offset Starting offset in memory
 * @param[in] size Size of buffer
 * @return The number of copied bytes.
 */
MORO8_PUBLIC(moro8_udword) moro8_get_memory(const struct moro8_vm* vm, moro8_uword* buffer, moro8_udword offset, moro8_udword size);

/**
 * Fills in the memory starting at offset with size bytes.
 *
 * @code
 * moro8_uword buffer[4] = { ... };
 * 
 * moro8_set_memory(vm, buffer, 0x1000, 4);
 * @endcode
 * @param[in] vm Some vm
 * @param[in] buffer Pointer to buffer containing the bytes to copy
 * @param[in] offset Starting offset in memory
 * @param[in] size Size of buffer
 * @return The number of copied bytes.
 */
MORO8_PUBLIC(moro8_udword) moro8_set_memory(struct moro8_vm* vm, const moro8_uword* buffer, moro8_udword offset, moro8_udword size);

/**
 * Gets a single byte from memory.
 *
 * @code 
 * moro8_set_memory_word(vm, 0x1000, 0xFF);
 *
 * // Output FF
 * printf("%02x", moro8_get_memory_word(vm, 0x1000));
 * @endcode
 * @param[in] vm Some vm
 * @param[in] address Memory address
 * @return Value from memory.
 */
MORO8_PUBLIC(moro8_uword) moro8_get_memory_word(struct moro8_vm* vm, moro8_udword address);

/**
 * Sets a single byte in memory.
 *
 * @code 
 * moro8_set_memory_word(vm, 0x1000, 0xFF);
 *
 * // Output FF
 * printf("%02x", moro8_get_memory_word(vm, 0x1000));
 * @endcode
 * @param[in] vm Some vm
 * @param[in] address Memory address
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_set_memory_word(struct moro8_vm* vm, moro8_udword address, moro8_uword value);

/**
 * Gets a double byte from memory.
 *
 * @code 
 * moro8_set_memory_dword(vm, 0x1000, 0xFF10);
 *
 * // Output 10FF
 * printf("%02x", moro8_get_memory_word(vm, 0x1000));
 * printf("%02x", moro8_get_memory_word(vm, 0x1001));
 *
 * // Output FF10
 * printf("%04x", moro8_get_memory_dword(vm, 0x1000));
 * @endcode
 * @param[in] vm Some vm
 * @param[in] address Memory address
 * @return Value from memory.
 */
MORO8_PUBLIC(moro8_udword) moro8_get_memory_dword(struct moro8_vm* vm, moro8_udword address);

/**
 * Sets a double byte in memory with the low byte first.
 *
 * @code 
 * moro8_set_memory_dword(vm, 0x1000, 0xFF10);
 *
 * // Output 10FF
 * printf("%02x", moro8_get_memory_word(vm, 0x1000));
 * printf("%02x", moro8_get_memory_word(vm, 0x1001));
 *
 * // Output FF10
 * printf("%04x", moro8_get_memory_dword(vm, 0x1000));
 * @endcode
 * @param[in] vm Some vm
 * @param[in] address Memory address
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_set_memory_dword(struct moro8_vm* vm, moro8_udword address, moro8_udword value);

MORO8_PUBLIC(int) moro8_load_state(struct moro8_vm* vm, const char* path);

MORO8_PUBLIC(int) moro8_dump_state(struct moro8_vm* vm, const char* path);

#if MORO8_WITH_PARSER

/**
 * Dumps the current vm state to an human-readable string.
 * 
 * @note
 * Available only if built with MORO8_WITH_PARSER=1
 * 
 * @param[in] vm Some vm
 * @return String representation of the vm state.
 */
MORO8_PUBLIC(char*) moro8_print(const struct moro8_vm* vm);

/**
 * Parses the string representation of a vm state.
 * 
 * @note
 * Available only if built with MORO8_WITH_PARSER=1
 * 
 * @param[out] vm Some vm
 * @param[in] buf Some buffer
 * @param[in] size Buffer size
 * @return The same vm pointer if there is no errors, NULL otherwise.
 */
MORO8_PUBLIC(struct moro8_vm*) moro8_parse(struct moro8_vm* vm, const char* buf, size_t size);

#endif

#if !MORO8_MINIMALIST

/**
 * Creates a snapshot from a vm.
 * @code
 * // Execute next instruction
 * moro8_step(vm);
 * 
 * // Backup the current state
 * moro8_vm* snapshot = moro8_create();
 * moro8_copy(snapshot, vm);
 * 
 * // Execute next instruction
 * moro8_step(vm);
 * 
 * // Restore previous state
 * moro8_copy(vm, snapshot);
 * moro8_delete(snapshot);
 * @endcode
 *
 * @note
 * Available only if built with MORO8_MINIMALIST=0
 * @param[out] snapshot VM to copy to
 * @param[in] vm VM to copy from
 */
MORO8_PUBLIC(void) moro8_copy(struct moro8_vm* snapshot, const struct moro8_vm* vm);

/**
 * Returns true only if the two vm are in the exact same state.
 * 
 * @note
 * Available only if built with MORO8_MINIMALIST=0
 * @param[in] left Some vm
 * @param[in] right Another vm
 * @return If the two vm are the same.
 */
MORO8_PUBLIC(int) moro8_equal(const struct moro8_vm* left, const struct moro8_vm* right);

#endif

#ifdef MORO8_DOXYGEN
/**
 * Gets the value in accumulator register.
 * @param[in] vm Some vm
 * @return Value from register.
 */
MORO8_PUBLIC(moro8_uword) moro8_get_ac(struct moro8_vm* vm) { return 0; }
/**
 * Gets the value in X register.
 * @param[in] vm Some vm
 * @return Value from register.
 */
MORO8_PUBLIC(moro8_uword) moro8_get_x(struct moro8_vm* vm) { return 0; }
/**
 * Gets the value in Y register.
 * @param[in] vm Some vm
 * @return Value from register.
 */
MORO8_PUBLIC(moro8_uword) moro8_get_y(struct moro8_vm* vm) { return 0; }
/**
 * Gets the value in stack pointer register.
 * @param[in] vm Some vm
 * @return Value from register.
 */
MORO8_PUBLIC(moro8_uword) moro8_get_sp(struct moro8_vm* vm) { return 0; }
/**
 * Sets the value in accumulator register.
 * @param[in] vm Some vm
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_set_ac(struct moro8_vm* vm, moro8_uword value) {}
/**
 * Sets the value in X register.
 * @param[in] vm Some vm
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_set_x(struct moro8_vm* vm, moro8_uword value) {}
/**
 * Sets the value in Y register.
 * @param[in] vm Some vm
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_set_y(struct moro8_vm* vm, moro8_uword value) {}
/**
 * Sets the value in stack pointer register.
 * @param[in] vm Some vm
 * @param[in] value New value
 */
MORO8_PUBLIC(void) moro8_set_sp(struct moro8_vm* vm, moro8_uword value) {}
#else
/** Get registers values */
#define moro8_get_ac(vm) moro8_get_register(vm, MORO8_REGISTER_AC)
#define moro8_get_x(vm) moro8_get_register(vm, MORO8_REGISTER_X)
#define moro8_get_y(vm) moro8_get_register(vm, MORO8_REGISTER_Y)
#define moro8_get_sp(vm) moro8_get_register(vm, MORO8_REGISTER_SP)
/** Set registers values */
#define moro8_set_ac(vm, value) moro8_set_register(vm, MORO8_REGISTER_AC, value)
#define moro8_set_x(vm, value) moro8_set_register(vm, MORO8_REGISTER_X, value)
#define moro8_set_y(vm, value) moro8_set_register(vm, MORO8_REGISTER_Y, value)
#define moro8_set_sp(vm, value) moro8_set_register(vm, MORO8_REGISTER_SP, value)
#endif

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

#ifdef __cplusplus
}
#endif

#endif
