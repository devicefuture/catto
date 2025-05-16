#ifndef CATTO_CONFIG_H_
#define CATTO_CONFIG_H_

#ifndef CATTO_USE_CUSTOM_TYPES

#include <stdint.h>

#ifdef CATTO_USE_64_BIT
    #define CATTO_BOOL int64_t
    #define CATTO_COUNT uint64_t
    #define CATTO_CHAR char
    #define CATTO_INT int64_t
    #define CATTO_FLOAT double

    #define CATTO_MAX_PRECISION 15
#else
    #define CATTO_BOOL int32_t
    #define CATTO_COUNT uint32_t
    #define CATTO_CHAR char
    #define CATTO_INT uint32_t
    #define CATTO_FLOAT float

    #define CATTO_MAX_PRECISION 6
#endif

#endif

#ifndef CATTO_FN_PREFIX
    #ifdef __cplusplus
        #define CATTO_FN_PREFIX inline
    #else
        #define CATTO_FN_PREFIX
    #endif
#endif

#define CATTO_USE_STDLIB

#ifdef CATTO_USE_STDLIB

#include <stdio.h>
#include <stdlib.h>

CATTO_FN_PREFIX void _catto_log(const char* text) {
    printf("%s", text);
}

CATTO_FN_PREFIX void _catto_logChar(char character) {
    printf("%c", character);
}

CATTO_FN_PREFIX void* _catto_malloc(CATTO_COUNT size) {
    return malloc(size);
}

CATTO_FN_PREFIX void* _catto_realloc(void* ptr, CATTO_COUNT size) {
    return realloc(ptr, size);
}

CATTO_FN_PREFIX void _catto_free(void* ptr) {
    free(ptr);
}

#define CATTO_LOG _catto_log
#define CATTO_LOG_CHAR _catto_logChar
#define CATTO_MALLOC _catto_malloc
#define CATTO_REALLOC _catto_realloc
#define CATTO_FREE _catto_free

#endif

#endif