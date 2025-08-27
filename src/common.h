typedef CATTO_BOOL catto_Bool;
typedef CATTO_COUNT catto_Count;
typedef CATTO_CHAR catto_Char;
typedef CATTO_INT catto_Int;
typedef CATTO_FLOAT catto_Float;

#define CATTO_TRUE 1
#define CATTO_FALSE 0
#define CATTO_NULL 0

#define CATTO_NAN (0.0 / 0.0)
#define CATTO_INFINITY (1.0 / 0.0)

#define CATTO_PI 3.141592653589793
#define CATTO_E 2.718281828459045
#define CATTO_PHI 1.618033988798948

#define CATTO_NEW(type) (type*)catto_safeMalloc(sizeof(type))

catto_Char* catto_safetyBuffer = CATTO_NULL;
catto_Bool catto_initialised = CATTO_FALSE;
catto_Bool catto_outOfMemory = CATTO_FALSE;

void catto_ensureSafetyBuffer() {
    if (!catto_safetyBuffer) {
        catto_safetyBuffer = (catto_Char*)CATTO_MALLOC(CATTO_SAFETY_BUFFER_SIZE);
    }
}

catto_Bool catto_init() {
    catto_ensureSafetyBuffer();

    catto_initialised = !!catto_safetyBuffer;

    return catto_initialised;
}

void* catto_safeMallocFallback(catto_Count size, catto_Count fallbackSize) {
    void* newPtr = CATTO_MALLOC(size);

    catto_ensureSafetyBuffer();

    if (!newPtr) {
        CATTO_FREE(catto_safetyBuffer);

        catto_safetyBuffer = CATTO_NULL;
        catto_outOfMemory = CATTO_TRUE;
        newPtr = CATTO_MALLOC(fallbackSize);
    }

    if (!newPtr) {
        #ifdef CATTO_PANIC
            CATTO_PANIC("Out of memory (unrecoverable)\n");
        #endif
    }

    return newPtr;
}

void* catto_safeMalloc(catto_Count size) {
    return catto_safeMallocFallback(size, size);
}

void* catto_safeReallocFallback(void* ptr, catto_Count size, catto_Count fallbackSize) {
    void* newPtr = CATTO_REALLOC(ptr, size);

    if (size == 0) {
        return newPtr;
    }

    catto_ensureSafetyBuffer();

    if (!newPtr) {
        CATTO_FREE(catto_safetyBuffer);

        catto_safetyBuffer = CATTO_NULL;
        catto_outOfMemory = CATTO_TRUE;
        newPtr = CATTO_REALLOC(ptr, fallbackSize);
    }

    if (!newPtr) {
        #ifdef CATTO_PANIC
            CATTO_PANIC("Out of memory (unrecoverable)\n");
        #endif
    }

    return newPtr;
}

void* catto_safeRealloc(void* ptr, catto_Count size) {
    return catto_safeReallocFallback(ptr, size, size);
}

void catto_copyMemory(const catto_Char* source, catto_Char* destination, catto_Count length, catto_Count offset) {
    for (catto_Count i = 0; i < length; i++) {
        destination[i + offset] = source[i];
    }
}

catto_Bool catto_memoryEquals(const catto_Char* a, const catto_Char* b, catto_Count length) {
    for (catto_Count i = 0; i < length; i++) {
        if (a[i] != b[i]) {
            return CATTO_FALSE;
        }
    }

    return CATTO_TRUE;
}