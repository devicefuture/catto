# `catto` 😺🖥️
The embeddable C runtime library for the [atto programming language](https://atto.devicefuture.org). It's used by the [DigiCron smartwatch](https://github.com/devicefuture/digicron) and [atto.js](https://github.com/devicefuture/atto.js).

## Building
To build the catto library, run:

```bash
./build.sh
```

The library will be built as a single header file in the `dist` directory, which will also include a header file used to configure catto. The interpreter and example code will also be built.

## Running the interpreter
To start the interpreter, run:

```bash
runtime/build/catto
```

To interrupt running programs, press <kbd>esc</kbd>. To exit the interpreter, type `exit`.

You can also run atto files by running this command, where `$FILE` is the atto file you want to run:

```bash
runtime/build/catto $FILE
```

Compile with `DEBUG_MEMORY` defined to get debug information about memory usage for every command entered.

## Running the example
To run the example code (found at `examples/hello.c`), run:

```bash
examples/build/hello
```

Compile with `TEST_MEMORY` defined to run the example in an infinite loop, which can then be used with a process monitor to check for memory leaks.