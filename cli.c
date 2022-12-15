#include "moro8.h"
#include "fs.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

typedef struct fs_file_iterator fs_file_iterator;
typedef struct moro8_cpu moro8_cpu;
typedef struct moro8_array_memory moro8_array_memory;

typedef enum moro8_format
{
    MORO8_FORMAT_BINARY,
    MORO8_FORMAT_HEX
} moro8_format;

static moro8_cpu *load_hex(moro8_cpu *cpu, fs_file_iterator *it)
{
    size_t col = 0;
    char c;
    moro8_uword word = 0;
    moro8_udword addr = MORO8_ROM_OFFSET;
    while (fs_next_char(it, &c))
    {
        switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            continue;
        default:
            if (c >= 'a' && c <= 'f')
            {
                c = c - 'a' + 'A';
            }

            if ((c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'))
            {
                word += ((c >= '0' && c <= '9') ? (c - '0') : (c - 'A' + 10)) << (((col + 1) % 2) * 4);
                ++col;
            }
            else
            {
                fprintf(stderr, "encountered an invalid character %c", c);
                return NULL;
            }

            if (col % 2 == 0)
            {
                moro8_set_memory_word(cpu, addr, (moro8_uword)word);
                word = 0;
                ++addr;
            }
            break;
        }
    }

    return cpu;
}

static void load_binary(moro8_cpu *cpu, fs_file_iterator *it)
{
    char c;
    moro8_udword addr = MORO8_ROM_OFFSET;
    while (fs_next_char(it, &c))
    {
        moro8_set_memory_word(cpu, addr, (moro8_uword)c);
        ++addr;
    }
}

static void run(const char *path, moro8_format format)
{
    fs_file_iterator *it = fs_iter_file(path);
    if (!it)
    {
        fprintf(stderr, "file not found %s\n", path);
        return;
    }

    printf("running %s...\n", path);

    // Initialize the cpu
    moro8_cpu cpu;
    moro8_init(&cpu);

    // Initialize the memory
    moro8_array_memory memory;
    moro8_array_memory_init(&memory);

    // Link cpu to memory
    moro8_set_memory_bus(&cpu, &memory.bus);

    // To initialize the cpu.
    moro8_uword prog[] = {};
    moro8_load(&cpu, NULL, 0);

    // Load program to memory.
    switch (format)
    {
    case MORO8_FORMAT_BINARY:
        load_binary(&cpu, it);
        break;
    case MORO8_FORMAT_HEX:
        if (!load_hex(&cpu, it))
        {
            fs_close_file(it);
            return;
        }
        break;
    default:
        break;
    }
    fs_close_file(it);

    // Run the program.
    moro8_run(&cpu);

    // Print memory
    char output[MORO8_PRINT_HEADER_BUFFER_SIZE];
    moro8_print(&cpu, output, MORO8_PRINT_HEADER_BUFFER_SIZE);
    printf("\n%.*s\n\n", MORO8_PRINT_HEADER_BUFFER_SIZE, output);
}

static int print_help()
{
    fprintf(stderr, "usage: moro8 [-h] [-f FORMAT] [target [target ...]]\n\
\n\
positional arguments:\n\
  target                input files\n\
\n\
optional arguments:\n\
  -h, --help            show this help message and exit\n\
  -f FORMAT, --format FORMAT\n\
                        input format (bin/hex)\n");
    return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
    moro8_format format = MORO8_FORMAT_BINARY;
    int last_arg = 0;
    int index_pos = 0;

    for (int i = argc - 1; i >= 0; --i)
    {
        if (argv[i][0] == '-')
        {
            last_arg = i;
            break;
        }
    }

    for (index_pos = 1; index_pos <= last_arg; ++index_pos)
    {
#define MORO8_HAS_ARG()      \
    if (index_pos >= argc)   \
    {                        \
        return print_help(); \
    }
#define MORO8_CHECK_ARG(s, n) strncmp(argv[index_pos], s, n) == 0
        if (MORO8_CHECK_ARG("--help", 6) || MORO8_CHECK_ARG("-h", 2))
        {
            return print_help();
        }
        else if (MORO8_CHECK_ARG("--format", 8) || MORO8_CHECK_ARG("-f", 2))
        {
            ++index_pos;
            MORO8_HAS_ARG()

            if (argv[index_pos][0] == '-')
            {
                return print_help();
            }
            else if (MORO8_CHECK_ARG("bin", 3))
            {
                format = MORO8_FORMAT_BINARY;
            }
            else if (MORO8_CHECK_ARG("hex", 3))
            {
                format = MORO8_FORMAT_HEX;
            }
            else
            {
                return print_help();
            }
        }
    }

    MORO8_HAS_ARG();
#undef MORO8_HAS_ARG
#undef MORO8_CHECK_ARG

    for (; index_pos < argc; ++index_pos)
    {
        run(argv[index_pos], format);
    }

    return 0;
}