# `catto` 😺🖥️
A C runtime library for the [atto programming language](https://atto.devicefuture.org).

## Building
To build the catto library, run:

```bash
./build.sh
```

The library will be built as a single header file in the `dist` directory. The interpreter and example code will also be built.

## Running the interpreter
To start the interpreter, run:

```bash
runtime/build/catto
```

To interrupt running programs, press <kbd>esc</kbd>. To exit the interpreter, type `exit`.

## Running the example
To run the example code (found at `examples/hello.c`), run:

```
examples/build/hello
```

Compile with `TEST_MEMORY` defined to run catto in an infinite loop, which can then be used with a process monitor to check for memory leaks.