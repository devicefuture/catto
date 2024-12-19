#!/bin/bash

CC=gcc

mkdir -p dist

echo -n > dist/catto.h

function include {
    (
        echo "// $1"
        echo
        cat $1
        echo
        echo
    ) >> dist/catto.h
}

cp src/config.h dist/catto-config.h

tee -a dist/catto.h > /dev/null << EOF
#ifndef CATTO_H_
#define CATTO_H_

EOF

include src/config.h
include src/common.h
include src/declarations.h
include src/operators.h
include src/contexts.h
include src/numbers.h
include src/strings.h
include src/datatypes.h
include src/tokeniser.h
include src/parser.h

tee -a dist/catto.h > /dev/null << EOF
#endif
EOF

mkdir -p runtime/build
mkdir -p examples/build

$CC -Idist/ runtime/catto.c -lncurses -o runtime/build/catto
$CC -Idist/ examples/hello.c -o examples/build/hello