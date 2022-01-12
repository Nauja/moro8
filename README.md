# moro8

[![CI](https://github.com/Nauja/libfs/actions/workflows/CI.yml/badge.svg)](https://github.com/Nauja/libfs/actions/workflows/CI.yml)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/Nauja/libfs/master/LICENSE)

Fantasy CPU.

## Build Manually

Activate latest **emsdk**:

```bash
cd /d F:\moro8
..\emscripten\emsdk\emsdk.bat activate latest
```

Compile to **wasm** and **js**:

```bash
emcc moro8.c -o moro8.js -s WASM=1 -s EXPORTED_FUNCTIONS=["_malloc","_free"] -s EXPORTED_RUNTIME_METHODS=ccall

emcc moro8.c -o moro8.js -s MODULARIZE -s EXPORT_NAME="moro8" -s NO_FILESYSTEM=1 -s EXPORTED_FUNCTIONS=["_malloc","_free"] -s EXPORTED_RUNTIME_METHODS=ccall

emcc moro8.c -o moro8.js -s MODULARIZE -s EXPORT_NAME="moro8" -s WASM=0 -s NO_FILESYSTEM=1 -s EXPORTED_FUNCTIONS=["_malloc","_free"] -s EXPORTED_RUNTIME_METHODS=ccall
```

Run locally:

```bash
emrun --browser chrome moro8.html
```

## Usage:

Include the module:

```html
<script src="moro8.js"></script>
```

Initialize the VM:

```js
var moro8_vm = Module.ccall('moro8_create', // name of C function
  null, // return type
  null, // argument types
  null); // arguments
```

Load a new program:

```js
Module.ccall('moro8_load', // name of C function
  null, // return type
  ["number", "array", "number"], // argument types
  [moro8_vm, [0, 2, 9, 3, 8, 1, 48], 7]); // arguments
```

Run your program:

```js
Module.ccall('moro8_run', // name of C function
  null, // return type
  ["number"], // argument types
  [moro8_vm]); // arguments
```

Get register value:

```js
var result = Module.ccall('moro8_get_register', // name of C function
  "number", // return type
  ["number", "number"], // argument types
  [moro8_vm, 0]); // arguments

console.log(`Result: ${result}`);
```

## License

Licensed under the [MIT](LICENSE) License.
