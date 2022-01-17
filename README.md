# moro8

[![CI](https://github.com/Nauja/moro8/actions/workflows/CI.yml/badge.svg)](https://github.com/Nauja/moro8/actions/workflows/CI.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/Nauja/moro8/master/LICENSE)

moro8 is a 8-bit fantasy CPU written in ANSI C with extensibility and portability in mind.

## Purpose

The ultimate goal of writing moro8 was to:

  * Learn more about 8-bit microcontrollers
  * Write small programs and games in assembler for fun (see [moro8asm](https://github.com/Nauja/moro8asm))
  * Make it run programs loaded from an SD card on an Arduino with 6KB of SRAM
  * Compile it to WASM so it can run on the Web (see [moro8.js](https://github.com/Nauja/moro8.js))

It is extensible because:

  * You can register hooks at runtime for handling additional opcodes providing new features
  * You can completely change how the memory is accessed and even implement some pagination

It is portable because:

  * Written in ANSI C with barely any dependencies on the system

## Usage

This is an example of running a program:

```c
#include "moro8.h"

struct moro8_vm vm;

moro8_uword prog[] = {
    0xA9, 0x02, // LDA #$02
    0x69, 0x03  // ADC #$03
};
moro8_load(&vm, prog, 4);
moro8_run(&vm);

printf("Result of 2 + 3 is %d", moro8_get_a(&vm));
```

## License

Licensed under the [MIT](LICENSE) License.
