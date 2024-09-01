#ifndef CATTO_H_
#define CATTO_H_

// src/config.h

#ifndef CATTO_CONFIG_H_
#define CATTO_CONFIG_H_

#ifndef CATTO_USE_CUSTOM_TYPES

#include <stdint.h>

#define CATTO_BOOL int
#define CATTO_COUNT unsigned int
#define CATTO_CHAR char
#define CATTO_INT int
#define CATTO_FLOAT float

#endif

#define CATTO_USE_STDLIB

#ifdef CATTO_USE_STDLIB

#include <stdio.h>
#include <stdlib.h>

void _catto_log_stdlib(char* text) {
    printf("%s", text);
}

void* _catto_malloc(CATTO_COUNT size) {
    return malloc(size);
}

void* _catto_realloc(void* ptr, CATTO_COUNT size) {
    return realloc(ptr, size);
}

void _catto_free(void* ptr) {
    free(ptr);
}

#define CATTO_LOG_STDLIB _catto_log_stdlib
#define CATTO_MALLOC _catto_malloc
#define CATTO_REALLOC _catto_realloc
#define CATTO_FREE _catto_free

#endif

#endif

// src/common.h

typedef CATTO_BOOL catto_Bool;
typedef CATTO_COUNT catto_Count;
typedef CATTO_CHAR catto_Char;

#define CATTO_TRUE 1
#define CATTO_FALSE 0
#define CATTO_NULL 0

#define CATTO_NEW(type) (type*)CATTO_MALLOC(sizeof(type))

// src/declarations.h

typedef struct catto_Context {
    catto_Bool isInitialised;
} catto_Context;

// src/contexts.h

catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->isInitialised = CATTO_FALSE;

    return context;
}

#endif
