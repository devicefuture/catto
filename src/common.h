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

void catto_copyMemory(const catto_Char* source, catto_Char* destination, catto_Count length, catto_Count offset) {
    for (catto_Count i = 0; i < length; i++) {
        destination[i + offset] = source[i];
    }
}

catto_Bool catto_memoryEquals(catto_Char* a, catto_Char* b, catto_Count length) {
    for (catto_Count i = 0; i < length; i++) {
        if (a[i] != b[i]) {
            return CATTO_FALSE;
        }
    }

    return CATTO_TRUE;
}