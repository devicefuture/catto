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

#define CATTO_NEW(type) (type*)CATTO_MALLOC(sizeof(type))

#define CATTO_MUST_C(condition, returnOnFail, cleanup) if (!(condition)) {cleanup; return returnOnFail;}
#define CATTO_MUST_R(condition, returnOnFail) if (!(condition)) {return returnOnFail;}
#define CATTO_MUST_N(condition) CATTO_MUST_R(condition, CATTO_NULL)
#define CATTO_MUST_F(condition) CATTO_MUST_R(condition, CATTO_FALSE)
#define CATTO_MUST_EC(condition, returnOnFail, cleanup) if (!(condition)) {context->errorState = CATTO_ERROR_STATE_OUT_OF_MEMORY; cleanup; return returnOnFail;}
#define CATTO_MUST_ER(condition, returnOnFail) if (!(condition)) {context->errorState = CATTO_ERROR_STATE_OUT_OF_MEMORY; return returnOnFail;}
#define CATTO_MUST_E(condition) CATTO_MUST_EC(condition, , )
#define CATTO_MUST(condition) CATTO_MUST_R(condition, )
#define CATTO_IGNORE(condition) (void)condition

#define CATTO_THROWS(result) __attribute__((warn_unused_result))

#define CATTO_TYPED_ZERO (catto_TypedValue) {.type = CATTO_DATA_TYPE_NUMBER, .value = {.asNumber = 0}}

#define CATTO_SAFE_ASSIGN(dest, type, src, check) do { \
        type CATTO_DEST = (type)src; \
        if (CATTO_DEST) {dest = CATTO_DEST;} \
        check; \
    } while (CATTO_FALSE);

#define CATTO_SAFE_REALLOC(pointer, type, newSize, check) do { \
        type CATTO_DEST = (type)CATTO_REALLOC(pointer, newSize); \
        if (CATTO_DEST) {pointer = CATTO_DEST;} \
        check; \
    } while (CATTO_FALSE);

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