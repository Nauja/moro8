# moro8

[![CI](https://github.com/Nauja/moro8/actions/workflows/CI.yml/badge.svg)](https://github.com/Nauja/moro8/actions/workflows/CI.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/Nauja/moro8/master/LICENSE)

moro8 is a fantasy CPU written in ANSI C with portability and minimalism in mind.

## Why ?

The ultimate goal of writing moro8 was to:

  * Learn more about 8bit cpu and microcontrollers
  * Write small programs and games in assembler for fun
  * Make it run programs loaded from an SD card on Arduino
  * Compile it to WASM so it can run on the Web (see [moro8.js](https://github.com/Nauja/moro8.js))

## Usage

This is an example of running a program:

```c
#include "moro8.h"

struct moro8_vm* vm = moro8_create();

moro8_uword prog[] = {
    0xA9, 0x02, // LDA #$02
    0x69, 0x03  // ADC #$03
};
moro8_load(vm, prog, 4);

printf("Result of 2 * 3 is %d", moro8_get_a(vm));

moro8_delete(vm);
```

## License

Licensed under the [MIT](LICENSE) License.
