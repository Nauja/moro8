# moro8

[![CI](https://github.com/Nauja/moro8/actions/workflows/CI.yml/badge.svg)](https://github.com/Nauja/moro8/actions/workflows/CI.yml)
[![CI Docs](https://github.com/Nauja/moro8/actions/workflows/CI_docs.yml/badge.svg)](https://github.com/Nauja/moro8/actions/workflows/CI_docs.yml)
[![Documentation Status](https://readthedocs.org/projects/moro8/badge/?version=latest)](https://moro8.readthedocs.io/en/latest/?badge=latest)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/Nauja/moro8/master/LICENSE)

moro8 is a 8-bit fantasy CPU written in ANSI C with extensibility and portability in mind.

## Purpose

The ultimate goal of writing moro8 was to:

  * Learn more about 8-bit microcontrollers
  * Write small programs and games in assembler for fun (see [moro8asm](https://github.com/Nauja/moro8asm))
  * Make it run programs loaded from an SD card on an Arduino with 6KB of SRAM
  * Compile it to WASM so it can run on the Web (see [moro8.js](https://github.com/Nauja/moro8.js))

It is extensible because:

  * You can register handlers at runtime for handling additional opcodes providing new features
  * You can completely change how the memory is accessed and even implement some pagination

It is portable because:

  * Written in ANSI C with barely any dependencies on the system

## Usage

This is how you can load a program to ROM and run it:

```c
#include "moro8.h"

// Initialize the cpu
struct moro8_cpu cpu;
moro8_init(&cpu);

// Initialize the memory
struct moro8_array_memory memory;
moro8_array_memory_init(&memory);

// Link cpu to memory
moro8_set_memory_bus(&cpu, &memory.bus);

// Load this small program to ROM and run
moro8_uword prog[] = {
    0xA9, 0x02, // LDA #$02
    0x69, 0x03  // ADC #$03
};
moro8_load(&cpu, prog, 4);
moro8_run(&cpu);

// Print result in accumulator register
printf("Result of 2 + 3 is %d", moro8_get_ac(&cpu));
```

You can notice that we have two distinct objects, the cpu itself, and the memory.
But why is that so ?

Well, while the cpu has a maximum memory of 64KB that could have easily been hard coded, I chose to design the library
so that the memory stays separated from the cpu and can easily be implemented in different ways. This is to allow for different
strategies such as some pagination system when running on a microcontroller with less than 64KB which is the case when
running on Arduino.

Check the [documentation](https://moro8.readthedocs.io/en/latest/) to find more examples and learn about the API.

## Build Manually

Copy the files [moro8.c](https://github.com/Nauja/moro8/blob/main/moro8.c) and [moro8.h](https://github.com/Nauja/moro8/blob/main/moro8.h) into an existing project.

Comment or uncomment the defines at the top of `moro8.h` depending on your configuration:

```c
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

...
```

You should now be able to compile this library correctly.

## Build with CMake

Tested with CMake >= 3.13.4:

```
git clone https://github.com/Nauja/moro8.git
cd moro8
git submodule init
git submodule update
mkdir build
cd build
cmake ..
```

CMake will correctly configure the defines at the top of [moro8.h](https://github.com/Nauja/moro8/blob/main/moro8.h) for your system.

You can then build this library manually as described above, or by using:

```
make
```

This will generate `moro8.a` if building as a static library and `libmoro8.so` in the `build` directory.

You can change the build process with a list of different options that you can pass to CMake. Turn them on with `On` and off with `Off`:
  * `-DMORO8_STATIC=On`: Enable building as static library. (on by default)
  * `-DMORO8_UNIT_TESTING=On`: Enable building the tests. (on by default)
  * `-DMORO8_DOXYGEN=On`: Enable building the docs. (off by default)
  * `-DMORO8_MINIMALIST=On`: Strip some extra functions. (off by default)
  * `-DMORO8_EXTENDED_OPCODES=On`: Enable extended opcodes specific to moro8. (on by default)
  * `-DMORO8_WITH_PARSER=On`: Enable moro8_print and moro8_parse functions. (on by default)
  * `-DMORO8_WITH_HANDLERS=On`: Enable support for custom opcode handlers. (on by default)
  * `-DMORO8_WITH_CLI=On`: Enable building the command-line interface. (on by default)

## Build with Visual Studio

Generate the Visual Studio solution with:

```
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
```

You can now open `build/moro8.sln` and compile the library.

## License

Licensed under the [MIT](https://github.com/Nauja/moro8/blob/main/LICENSE) License.
