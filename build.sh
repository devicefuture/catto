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
include src/cattox.h
include src/operators.h
include src/contexts.h
include src/numbers.h
include src/strings.h
include src/lists.h
include src/datatypes.h
include src/tokeniser.h
include src/parser.h
include src/tokenfiles.h
include src/stdlib/controlflow.h
include src/stdlib/io.h
include src/stdlib/lists.h
include src/stdlib/functions.h
include src/stdlib/stdlib.h
include src/extensions/test.h
include src/extensions/csv.h

tee -a dist/catto.h > /dev/null << EOF
#endif
EOF

sed -i "s/^\([a-zA-Z_][a-zA-Z0-9_]*\** .*).*{\)/CATTO_FN_PREFIX \1/g" dist/catto.h dist/catto-config.h
sed -i "s/^\([a-zA-Z_][a-zA-Z0-9_]*\** .*=\)/CATTO_FN_PREFIX \1/g" dist/catto.h dist/catto-config.h
sed -i "s/^\(#define [^)]*) *\)\([a-zA-Z_][a-zA-Z0-9_]*.*(.*{\)/\1CATTO_FN_PREFIX \2/g" dist/catto.h dist/catto-config.h

mkdir -p runtime/build
mkdir -p examples/build

$CC -Idist/ $1 -DCATTO_USE_64_BIT runtime/catto.c -o runtime/build/catto
$CC -Idist/ examples/hello.c -o examples/build/hello