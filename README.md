# `catto` 😺🖥️
A C runtime library for the [atto programming language](https://atto.devicefuture.org).

## Building
To build catto, run:

```bash
./build.sh
```

## Running the interpreter
To start the interpreter, run:

```bash
runtime/build/catto
```

## Running the example
To run the example code (found at `examples/hello.c`), run:

```
examples/build/hello
```

Compile with `TEST_MEMORY` defined to run catto in an infinite loop, which can then be used with a process monitor to check for memory leaks.