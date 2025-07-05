#ifndef CATTO_H_
#define CATTO_H_

// src/config.h

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
    #define CATTO_EPSILON 1E-15
    #define CATTO_LOG2_ITERATIONS 47
    #define CATTO_EXP2_ITERATIONS 16
    #define CATTO_SQRT_ITERATIONS 55
    #define CATTO_COS_ITERATIONS 18
#else
    #define CATTO_BOOL int32_t
    #define CATTO_COUNT uint32_t
    #define CATTO_CHAR char
    #define CATTO_INT int32_t
    #define CATTO_FLOAT float

    #define CATTO_MAX_PRECISION 6
    #define CATTO_EPSILON 1E-6
    #define CATTO_LOG2_ITERATIONS 47
    #define CATTO_EXP2_ITERATIONS 6
    #define CATTO_SQRT_ITERATIONS 27
    #define CATTO_COS_ITERATIONS 10
#endif

#endif

#ifndef CATTO_FN_PREFIX
    #ifdef __cplusplus
        #define CATTO_FN_PREFIX inline
    #else
        #define CATTO_FN_PREFIX
    #endif
#endif

#ifndef CATTO_NOSTDLIB

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

// src/common.h

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

// src/declarations.h

typedef enum {
    CATTO_ERROR_STATE_NONE = 0,
    CATTO_ERROR_STATE_UNEXPECTED_TOKEN,
    CATTO_ERROR_STATE_NO_RETURN,
    CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK,
    CATTO_ERROR_STATE_MISMATCHED_CLOSING_MARK,
    CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP,
    CATTO_ERROR_STATE_UNKNOWN_PROCEDURE,
    CATTO_ERROR_STATE_NOT_A_FUNCTION,
    CATTO_ERROR_STATE_NOT_A_LIST,
    CATTO_ERROR_STATE_INVALID_LIST_VALUE,
    CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE
} catto_ErrorState;

typedef enum {
    CATTO_DATA_TYPE_NULL = '\0',
    CATTO_DATA_TYPE_NUMBER = '%',
    CATTO_DATA_TYPE_STRING = '$',
    CATTO_DATA_TYPE_FUNCTION = 'f',
    CATTO_DATA_TYPE_LIST = 'l'
} catto_DataType;

typedef enum {
    CATTO_MARK_SEARCH_ALL,
    CATTO_MARK_SEARCH_LOOP_ONLY
} catto_MarkSearchMode;

typedef enum {
    CATTO_TRIG_MODE_RADIANS,
    CATTO_TRIG_MODE_DEGREES,
    CATTO_TRIG_MODE_GRADIANS,
    CATTO_TRIG_MODE_TURNS
} catto_TrigMode;

typedef struct catto_Context {
    struct catto_CommandHandler* firstCommandHandler;
    struct catto_CommandHandler* lastCommandHandler;
    struct catto_Variable* firstVariable;
    struct catto_Variable* lastVariable;
    struct catto_Procedure* firstProcedure;
    struct catto_Procedure* lastProcedure;
    struct catto_AstNode* firstParsedStatement;
    struct catto_AstNode* currentParsedStatement;
    struct catto_AstNode* nextParsedStatement;
    struct catto_AstNode* firstParsedArgument;
    struct catto_AstNode* nextParsedArgument;
    struct catto_AstNode** statementStack;
    catto_Count statementStackCount;
    void** pointersToGc;
    catto_DataType* pointerTypesToGc;
    catto_Count pointersToGcCount;
    catto_ErrorState errorState;
    catto_Count subjectLineNumber;
    catto_Bool scrawlMode;
    catto_TrigMode trigMode;
    void* userData;
} catto_Context;

typedef void (*catto_CommandHandlerFunction)(catto_Context* context);
typedef struct catto_TypedValue (*catto_FunctionHandlerFunction)(catto_Context* context, catto_DataType returnType);

typedef struct catto_CommandHandler {
    const catto_Char* name;
    catto_CommandHandlerFunction function;
    struct catto_CommandHandler* nextCommandHandler;
} catto_CommandHandler;

typedef enum {
    CATTO_TOKEN_TYPE_SYNTAX_ERROR = '\0',
    CATTO_TOKEN_TYPE_NEXT_LINE = 'N',
    CATTO_TOKEN_TYPE_LINE_NUMBER = 'L',
    CATTO_TOKEN_TYPE_COMMENT = '#',
    CATTO_TOKEN_TYPE_COMMAND = 'c',
    CATTO_TOKEN_TYPE_STRING = '$',
    CATTO_TOKEN_TYPE_NUMBER = '%',
    CATTO_TOKEN_TYPE_IDENTIFIER = 'x',
    CATTO_TOKEN_TYPE_DELIMETER = ',',
    CATTO_TOKEN_TYPE_STATEMENT_DELIMETER = ':',
    CATTO_TOKEN_TYPE_OPERATOR = '+',
    CATTO_TOKEN_TYPE_OPENING_BRACKET = '(',
    CATTO_TOKEN_TYPE_CLOSING_BRACKET = ')',
    CATTO_TOKEN_TYPE_OPENING_ACCESSOR_BRACKET = '[',
    CATTO_TOKEN_TYPE_CLOSING_ACCESSOR_BRACKET = ']'
} catto_TokenType;

typedef struct catto_Token {
    catto_TokenType type;
    union {
        catto_Count asCodeIndex;
        catto_Count asLineNumber;
        catto_Float asNumber;
        catto_Char* asString;
        const catto_Char* asConstString;
        catto_CommandHandler* asCommandHandler;
    } value;
    struct catto_Token* nextToken;
} catto_Token;

typedef struct catto_List {
    struct catto_TypedValue* values;
    catto_Count length;
    catto_Count referenceCount;
} catto_List;

typedef struct catto_TypedValue {
    catto_DataType type;
    union {
        catto_Float asNumber;
        catto_Char* asString;
        catto_List* asList;
        catto_FunctionHandlerFunction asFunction;
    } value;
} catto_TypedValue;

typedef struct catto_Variable {
    catto_Char* name;
    catto_Count scope;
    catto_TypedValue value;
    struct catto_AstNode* argumentReference;
    struct catto_Variable* nextVariable;
} catto_Variable;

typedef struct catto_Procedure {
    catto_Char* name;
    catto_Char** parameterNames;
    catto_Count parameterCount;
    struct catto_AstNode* astNode;
    struct catto_Procedure* nextProcedure;
} catto_Procedure;

typedef enum {
    CATTO_AST_NODE_TYPE_SYNTAX_ERROR = '\0',
    CATTO_AST_NODE_TYPE_NOOP = 'n',
    CATTO_AST_NODE_TYPE_COMMAND_STATEMENT = 'c',
    CATTO_AST_NODE_TYPE_PROCEDURE_STATEMENT = 'p',
    CATTO_AST_NODE_TYPE_ASSIGNMENT_STATEMENT = '=',
    CATTO_AST_NODE_TYPE_EXPRESSION_LEAF = 'e',
    CATTO_AST_NODE_TYPE_UNARY_EXPRESSION = '-',
    CATTO_AST_NODE_TYPE_BINARY_EXPRESSION = '+'
} catto_AstNodeType;

typedef struct catto_AstNode {
    catto_AstNodeType type;
    union {
        struct {
            catto_Count lineNumber;
            struct catto_AstNode* firstArgument;
            union {
                catto_CommandHandler* asCommandHandler;
                struct {
                    catto_Char* subjectVariable;
                    struct catto_AstNode* index;
                } asAssignee;
                struct {
                    catto_Char* name;
                } asProcedure;
            } attributes;
            struct catto_AstNode* previousAstNode;
        } asStatement;
        struct {
            catto_TypedValue* value;
            catto_Char* subjectVariable;
            struct catto_AstNode* firstArgument;
            struct catto_AstNode* index;
            catto_Bool appendFlag;
        } asExpressionLeaf;
        struct {
            struct catto_AstNode* child;
            catto_Char* operatorValue;
        } asUnaryExpression;
        struct {
            struct catto_AstNode* firstChild;
            catto_Char** operatorValues;
        } asBinaryExpression;
    } value;
    struct catto_AstNode* nextAstNode;
} catto_AstNode;

typedef catto_TypedValue (*catto_UnaryOperatorFunction)(catto_Context* context, catto_TypedValue value);
typedef catto_TypedValue (*catto_BinaryOperatorFunction)(catto_Context* context, catto_TypedValue a, catto_TypedValue b);

typedef struct catto_OperatorMapping {
    const catto_Char* operatorValue;
    catto_UnaryOperatorFunction unaryFunction;
    catto_BinaryOperatorFunction binaryFunction;
} catto_OperatorMapping;

typedef enum {
    CATTO_SLICING_METHOD_LEFT,
    CATTO_SLICING_METHOD_RIGHT,
    CATTO_SLICING_METHOD_MID
} catto_SlicingMethod;

catto_Context* catto_newContext();
void catto_freeContext(catto_Context* context);
void catto_addPointerToGc(catto_Context* context, catto_DataType type, void* ptr);
void catto_removePointerFromGc(catto_Context* context, void* ptr);
void catto_gc(catto_Context* context);
void catto_addCommand(catto_Context* context, const catto_Char* name, catto_CommandHandlerFunction function);
void catto_addFunction(catto_Context* context, const catto_Char* name, catto_FunctionHandlerFunction function);
catto_DataType catto_removeTypeFromVariableName(catto_Char* name);
catto_Variable* catto_getVariableObject(catto_Context* context, catto_Char* name, catto_Bool allowOutsideScope);
catto_TypedValue* catto_getVariable(catto_Context* context, catto_Char* name);
catto_Variable* catto_setVariableScoped(catto_Context* context, const catto_Char* name, catto_TypedValue value, catto_Bool allowOutsideScope);
catto_Variable* catto_setVariable(catto_Context* context, const catto_Char* name, catto_TypedValue value);
catto_Procedure* catto_getProcedure(catto_Context* context, const catto_Char* name);
catto_Procedure* catto_createProcedure(catto_Context* context, const catto_Char* name);
void catto_assignValue(catto_Context* context, catto_AstNode* astNode, catto_TypedValue value);
catto_Bool catto_hasNextArg(catto_Context* context);
catto_TypedValue catto_evalExpression(catto_Context* context, catto_AstNode* astNode);
catto_AstNode* catto_getNextArg(catto_Context* context);
catto_TypedValue catto_evalNextArg(catto_Context* context);
catto_Bool catto_step(catto_Context* context);
void catto_goto(catto_Context* context, catto_Count lineNumber);
void catto_pushOntoStatementStack(catto_Context* context, catto_AstNode* statement);
catto_AstNode* catto_popFromStatementStack(catto_Context* context);
void catto_load(catto_Context* context, const catto_Char* code);
void catto_run(catto_Context* context);

void catto_addContextStandardCommands(catto_Context* context);

catto_Float catto_ln(catto_Float value);
catto_Float catto_log(catto_Float value);
catto_Float catto_log2(catto_Float value);
catto_Float catto_exp2(catto_Float value);
catto_Float catto_power(catto_Float base, catto_Float power);
catto_Float catto_sqrt(catto_Float value);
catto_Float catto_fromRadians(catto_Float value, catto_TrigMode trigMode);
catto_Float catto_toRadians(catto_Float value, catto_TrigMode trigMode);
catto_Float catto_sin(catto_Float value);
catto_Float catto_cos(catto_Float value);
catto_Float catto_tan(catto_Float value);
catto_Float catto_asin(catto_Float value);
catto_Float catto_acos(catto_Float value);
catto_Float catto_atan(catto_Float value);
catto_Float catto_roundToPrecision(catto_Float number, catto_Count precision);
catto_Char* catto_numberToString(catto_Float number);

catto_Count catto_stringLength(const catto_Char* string);
catto_Bool catto_stringsEqual(const catto_Char* a, const catto_Char* b);
catto_Bool catto_stringsEqualCaseInsensitive(const catto_Char* a, const catto_Char* b);
catto_Char* catto_copyString(const catto_Char* string);
catto_Char* catto_appendCharToString(catto_Char* string, catto_Char character);
catto_Char* catto_appendToString(catto_Char* a, const catto_Char* b);
catto_Char* catto_reverseString(catto_Char* string);
catto_Bool catto_stringStartsWith(const catto_Char* a, const catto_Char* b);
catto_Float catto_unsignedStringToNumber(const catto_Char* string, catto_Count* charactersEaten);
catto_Float catto_unsignedStringToBaseNumber(const catto_Char* string, catto_Count base, catto_Count* charactersEaten);
catto_Float catto_stringToNumber(const catto_Char* string, catto_Count* charactersEaten);
catto_Float catto_stringToBaseNumber(const catto_Char* string, catto_Count base, catto_Count* charactersEaten);

catto_List* catto_newList();
catto_List* catto_referenceList(catto_List* list);
void catto_dereferenceList(catto_Context* context, catto_List* list);
void catto_freeList(catto_Context* context, catto_List* list);
void catto_pushOntoList(catto_List* list, catto_TypedValue value);
catto_TypedValue catto_popFromList(catto_Context* context, catto_List* list);
void catto_insertIntoList(catto_List* list, catto_TypedValue value, catto_Count index);
catto_TypedValue catto_removeFromList(catto_Context* context, catto_List* list, catto_Count index);
catto_TypedValue catto_getListItem(catto_List* list, catto_Count index);
void catto_setListItem(catto_Context* context, catto_List* list, catto_Count index, catto_TypedValue value);
catto_Char* catto_listToString(catto_List* list);

catto_Float catto_asNumber(catto_TypedValue value);
catto_TypedValue catto_asTypedNumber(catto_Float value);
catto_Char* catto_asString(catto_TypedValue value);
catto_TypedValue catto_asTypedString(catto_Char* value);
catto_Bool catto_asBool(catto_TypedValue value);
void catto_freeTypedValue(catto_TypedValue* valuePtr);
catto_TypedValue catto_copyTypedValue(catto_TypedValue value);
catto_TypedValue catto_castTypedValue(catto_TypedValue value, catto_DataType type);
void catto_addTypedValueToGc(catto_Context* context, catto_TypedValue value);
void catto_removeTypedValueFromGc(catto_Context* context, catto_TypedValue value);

catto_Token* catto_tokenise(catto_Context* context, const catto_Char* code);
void catto_freeTokens(catto_Token* firstToken);
void catto_debugTokens(catto_Token* firstToken);

catto_AstNode* catto_createExpressionLeaf(catto_TypedValue value, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parse(catto_Token* firstToken);
catto_Bool catto_isCommand(catto_AstNode* astNode, const catto_Char* command);
catto_TypedValue* catto_getMarkConditionSwitch(catto_AstNode* astNode);
catto_Bool catto_markConditionSwitchIsEnabled(catto_AstNode* astNode);
catto_Bool catto_setMarkConditionSwitch(catto_AstNode* astNode, catto_Bool enabled);
catto_Bool catto_isOpeningMark(catto_AstNode* astNode, catto_MarkSearchMode searchMode);
catto_Bool catto_isClosingMark(catto_AstNode* astNode, catto_MarkSearchMode searchMode);
catto_AstNode* catto_findOpeningMark(catto_AstNode* astNode, const catto_Char* mark, catto_MarkSearchMode searchMode);
catto_AstNode* catto_findClosingMark(catto_AstNode* astNode, const catto_Char* mark, catto_MarkSearchMode searchMode);
void catto_freeAstNodes(catto_AstNode* firstAstNode);
void catto_debugAstNodes(catto_AstNode* firstAstNode);

// src/operators.h

CATTO_FN_PREFIX const catto_Char* catto_operators[] = {
    "+", "-", "*", "/", "div", "mod", "^",
    "&", "|", "~", "¬",
    "<<", ">>",
    "!=", "<=", ">=", "=", "<", ">",
    "and", "or", "xor", "not",
    ";",
    CATTO_NULL
};

CATTO_FN_PREFIX const catto_Char** catto_operatorPrecedence[] = {
    (const catto_Char*[]) {"and", "or", "xor", CATTO_NULL},
    (const catto_Char*[]) {"!=", "<=", ">=", "=", "<", ">", CATTO_NULL},
    (const catto_Char*[]) {";", CATTO_NULL},
    (const catto_Char*[]) {"<<", ">>", CATTO_NULL},
    (const catto_Char*[]) {"+", "-", CATTO_NULL},
    (const catto_Char*[]) {"*", "/", "div", "mod", CATTO_NULL},
    (const catto_Char*[]) {"^", CATTO_NULL},
    (const catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    CATTO_NULL
};

CATTO_FN_PREFIX const catto_Char* catto_unaryOperators[] = {
    "+",
    "-",
    "¬",
    "not",
    CATTO_NULL
};

#define CATTO_UNARY_NUMERIC_OPERATOR(name, operator) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_TypedValue value) { \
        return catto_asTypedNumber(operator catto_asNumber(value)); \
    }

#define CATTO_UNARY_INTEGER_OPERATOR(name, operator) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_TypedValue value) { \
        return catto_asTypedNumber(operator (catto_Int)catto_asNumber(value)); \
    }

#define CATTO_BINARY_NUMERIC_OPERATOR(name, operator) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber(catto_asNumber(a) operator catto_asNumber(b)); \
    }

#define CATTO_BINARY_INTEGER_OPERATOR(name, operator) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber((catto_Int)catto_asNumber(a) operator (catto_Int)catto_asNumber(b)); \
    }

#define CATTO_BINARY_LOGICAL_OPERATOR(name, operator) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber(((catto_Int)catto_asNumber(a) operator (catto_Int)catto_asNumber(b)) ? 1 : 0); \
    }

CATTO_UNARY_NUMERIC_OPERATOR(catto_unary_add, +);
CATTO_UNARY_NUMERIC_OPERATOR(catto_unary_subtract, -);
CATTO_UNARY_INTEGER_OPERATOR(catto_unary_bitwiseNot, ~);
CATTO_UNARY_INTEGER_OPERATOR(catto_unary_not, !);

CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_add, +);
CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_subtract, -);
CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_multiply, *);
CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_divide, /);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_integerDivide, /);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_modulo, %);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitwiseAnd, &);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitwiseOr, |);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitwiseXor, ^);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitShiftLeft, <<);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitShiftRight, >>);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_notEqual, !=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_lessThanOrEqual, <=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_greaterThanOrEqual, >=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_lessThan, <);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_greaterThan, >);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_and, &&);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_or, ||);

CATTO_FN_PREFIX catto_TypedValue catto_binary_power(catto_Context* context, catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(catto_power(catto_asNumber(a), catto_asNumber(b)));
}

CATTO_FN_PREFIX catto_TypedValue catto_binary_xor(catto_Context* context, catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(((catto_Int)catto_asNumber(a) ^ (catto_Int)catto_asNumber(b)) ? 1 : 0);
}

CATTO_FN_PREFIX catto_TypedValue catto_binary_equal(catto_Context* context, catto_TypedValue a, catto_TypedValue b) {
    if (a.type == CATTO_DATA_TYPE_NUMBER && b.type == CATTO_DATA_TYPE_NUMBER) {
        return catto_asTypedNumber(catto_asNumber(a) == catto_asNumber(b) ? 1 : 0);
    }

    catto_Char* aString = catto_asString(a);
    catto_Char* bString = catto_asString(b);

    catto_TypedValue result = catto_asTypedNumber(catto_stringsEqual(aString, bString) ? 1 : 0);

    CATTO_FREE(aString);
    CATTO_FREE(bString);

    return result;
}

CATTO_FN_PREFIX catto_TypedValue catto_binary_concat(catto_Context* context, catto_TypedValue a, catto_TypedValue b) {
    catto_Char* resultString = catto_copyString("");
    catto_Char* aString = catto_asString(a);
    catto_Char* bString = catto_asString(b);

    resultString = catto_appendToString(resultString, aString);
    resultString = catto_appendToString(resultString, bString);

    CATTO_FREE(aString);
    CATTO_FREE(bString);

    catto_addPointerToGc(context, CATTO_DATA_TYPE_STRING, resultString);

    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_STRING,
        .value = {.asString = resultString}
    };
}

CATTO_FN_PREFIX catto_OperatorMapping catto_operatorMappings[] = {
    {"+", catto_unary_add, catto_binary_add},
    {"-", catto_unary_subtract, catto_binary_subtract},
    {"*", CATTO_NULL, catto_binary_multiply},
    {"/", CATTO_NULL, catto_binary_divide},
    {"div", CATTO_NULL, catto_binary_integerDivide},
    {"mod", CATTO_NULL, catto_binary_modulo},
    {"^", CATTO_NULL, catto_binary_power},
    {"&", CATTO_NULL, catto_binary_bitwiseAnd},
    {"|", CATTO_NULL, catto_binary_bitwiseOr},
    {"~", CATTO_NULL, catto_binary_bitwiseXor},
    {"¬", catto_unary_bitwiseNot, CATTO_NULL},
    {"<<", CATTO_NULL, catto_binary_bitShiftLeft},
    {">>", CATTO_NULL, catto_binary_bitShiftRight},
    {"!=", CATTO_NULL, catto_binary_notEqual},
    {"<=", CATTO_NULL, catto_binary_lessThanOrEqual},
    {">=", CATTO_NULL, catto_binary_greaterThanOrEqual},
    {"=", CATTO_NULL, catto_binary_equal},
    {"<", CATTO_NULL, catto_binary_lessThan},
    {">", CATTO_NULL, catto_binary_greaterThan},
    {"and", CATTO_NULL, catto_binary_and},
    {"or", CATTO_NULL, catto_binary_or},
    {"xor", CATTO_NULL, catto_binary_xor},
    {"not", catto_unary_not, CATTO_NULL},
    {";", CATTO_NULL, catto_binary_concat},
    {CATTO_NULL, CATTO_NULL, CATTO_NULL}
};

// src/contexts.h

CATTO_FN_PREFIX catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->firstCommandHandler = CATTO_NULL;
    context->lastCommandHandler = CATTO_NULL;
    context->firstVariable = CATTO_NULL;
    context->lastVariable = CATTO_NULL;
    context->firstProcedure = CATTO_NULL;
    context->lastProcedure = CATTO_NULL;

    context->firstParsedStatement = CATTO_NULL;
    context->nextParsedStatement = CATTO_NULL;
    context->firstParsedArgument = CATTO_NULL;
    context->nextParsedArgument = CATTO_NULL;

    context->statementStack = (catto_AstNode**)CATTO_MALLOC(0);
    context->statementStackCount = 0;

    context->pointersToGc = (void**)CATTO_MALLOC(0);
    context->pointersToGcCount = 0;

    context->errorState = CATTO_ERROR_STATE_NONE;
    context->subjectLineNumber = 0;
    context->scrawlMode = CATTO_FALSE;
    context->trigMode = CATTO_TRIG_MODE_DEGREES;

    return context;
}

CATTO_FN_PREFIX void catto_freeContext(catto_Context* context) {
    catto_CommandHandler* commandHandler = context->firstCommandHandler;

    while (commandHandler) {
        catto_CommandHandler* nextCommandHandler = commandHandler->nextCommandHandler;

        CATTO_FREE(commandHandler);

        commandHandler = nextCommandHandler;
    }

    catto_Variable* variable = context->firstVariable;

    while (variable) {
        catto_Variable* nextVariable = variable->nextVariable;

        catto_addTypedValueToGc(context, variable->value);

        CATTO_FREE(variable->name);
        CATTO_FREE(variable);

        variable = nextVariable;
    }

    catto_Procedure* procedure = context->firstProcedure;

    while (procedure) {
        catto_Procedure* nextProcedure = procedure->nextProcedure;

        CATTO_FREE(procedure->name);
        CATTO_FREE(procedure->parameterNames);
        CATTO_FREE(procedure);

        procedure = nextProcedure;
    }

    catto_freeAstNodes(context->firstParsedStatement);

    catto_gc(context);

    CATTO_FREE(context->statementStack);
    CATTO_FREE(context->pointersToGc);
    CATTO_FREE(context->pointerTypesToGc);
    CATTO_FREE(context);
}

CATTO_FN_PREFIX void catto_addPointerToGc(catto_Context* context, catto_DataType type, void* ptr) {
    catto_removePointerFromGc(context, ptr);

    context->pointersToGc = (void**)CATTO_REALLOC(context->pointersToGc, sizeof(void*) * (context->pointersToGcCount + 1));
    context->pointersToGc[context->pointersToGcCount] = ptr;

    context->pointerTypesToGc = (catto_DataType*)CATTO_REALLOC(context->pointerTypesToGc, sizeof(catto_DataType) * (context->pointersToGcCount + 1));
    context->pointerTypesToGc[context->pointersToGcCount] = type;

    context->pointersToGcCount++;
}

CATTO_FN_PREFIX void catto_removePointerFromGc(catto_Context* context, void* ptr) {
    if (!ptr || context->pointersToGcCount == 0) {
        return;
    }

    for (catto_Count i = 0; i < context->pointersToGcCount; i++) {
        if (context->pointersToGc[i] == ptr) {
            context->pointerTypesToGc[i] = CATTO_NULL;
        }
    }
}

CATTO_FN_PREFIX void catto_gc(catto_Context* context) {
    if (context->pointersToGcCount == 0) {
        return;
    }

    for (catto_Count i = 0; i < context->pointersToGcCount; i++) {
        void* ptr = context->pointersToGc[i];
        catto_DataType type = context->pointerTypesToGc[i];

        if (!ptr) {
            continue;
        }

        if (type == CATTO_DATA_TYPE_LIST) {
            catto_List* list = (catto_List*)ptr;

            if (list->referenceCount > 0) {
                continue;
            }

            catto_freeList(context, list);
        } else {
            CATTO_FREE(ptr);
        }
    }

    context->pointersToGc = (void**)CATTO_REALLOC(context->pointersToGc, 0);
    context->pointerTypesToGc = (catto_DataType*)CATTO_REALLOC(context->pointerTypesToGc, 0);
    context->pointersToGcCount = 0;
}

CATTO_FN_PREFIX void catto_addCommand(catto_Context* context, const catto_Char* name, catto_CommandHandlerFunction function) {
    catto_CommandHandler* commandHandler = CATTO_NEW(catto_CommandHandler);

    commandHandler->name = name;
    commandHandler->function = function;
    commandHandler->nextCommandHandler = CATTO_NULL;

    if (!context->firstCommandHandler) {
        context->firstCommandHandler = commandHandler;
    }

    if (context->lastCommandHandler) {
        context->lastCommandHandler->nextCommandHandler = commandHandler;
    }

    context->lastCommandHandler = commandHandler;
}

CATTO_FN_PREFIX void catto_addFunction(catto_Context* context, const catto_Char* name, catto_FunctionHandlerFunction function) {
    catto_setVariable(context, name, (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_FUNCTION,
        .value = {.asFunction = function}
    });
}

CATTO_FN_PREFIX catto_DataType catto_removeTypeFromVariableName(catto_Char* name) {
    catto_Count i = 0;

    while (name[i]) {
        if (name[i] == '$' || name[i] == '%') {
            catto_DataType type = (catto_DataType)name[i];

            name[i] = CATTO_NULL;

            return type;
        }

        i++;
    }

    return CATTO_DATA_TYPE_NULL;
}

CATTO_FN_PREFIX catto_Variable* catto_getVariableObject(catto_Context* context, catto_Char* name, catto_Bool allowOutsideScope) {
    catto_Variable* currentVariable = context->firstVariable;

    if (context->statementStackCount > 0) {
        // Search for a local variable first

        while (currentVariable) {
            if (currentVariable->scope == context->statementStackCount && catto_stringsEqualCaseInsensitive(currentVariable->name, name)) {
                return currentVariable;
            }

            currentVariable = currentVariable->nextVariable;
        }

        if (!allowOutsideScope) {
            return CATTO_NULL;
        }

        // No local variable found; search for a global variable

        currentVariable = context->firstVariable;
    }

    while (currentVariable) {
        if (currentVariable->scope == 0 && catto_stringsEqualCaseInsensitive(currentVariable->name, name)) {
            return currentVariable;
        }

        currentVariable = currentVariable->nextVariable;
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_TypedValue* catto_getVariable(catto_Context* context, catto_Char* name) {
    catto_Variable* variable = catto_getVariableObject(context, name, CATTO_TRUE);

    if (!variable) {
        return CATTO_NULL;
    }

    return &(variable->value);
}

CATTO_FN_PREFIX catto_Variable* catto_setVariableScoped(catto_Context* context, const catto_Char* name, catto_TypedValue value, catto_Bool allowOutsideScope) {
    catto_Char* untypedName = catto_copyString(name);
    catto_DataType type = (catto_DataType)catto_removeTypeFromVariableName(untypedName);
    catto_Variable* existingVariable = catto_getVariableObject(context, untypedName, allowOutsideScope);

    if (existingVariable && existingVariable->argumentReference) {
        catto_Bool affectLowerScope = context->statementStackCount > 0;

        if (affectLowerScope) {
            context->statementStackCount--;
        }

        catto_assignValue(context, existingVariable->argumentReference, value);

        if (affectLowerScope) {
            context->statementStackCount++;
        }
    }

    if (existingVariable) {
        catto_TypedValue* existingVariableValue = &(existingVariable->value);

        catto_addTypedValueToGc(context, *existingVariableValue);

        *existingVariableValue = catto_copyTypedValue(value);

        CATTO_FREE(untypedName);

        return existingVariable;
    } else {
        catto_Variable* variable = CATTO_NEW(catto_Variable);

        variable->name = untypedName;
        variable->scope = context->statementStackCount;
        variable->value = catto_copyTypedValue(value);
        variable->argumentReference = CATTO_NULL;
        variable->nextVariable = CATTO_NULL;

        if (!context->firstVariable) {
            context->firstVariable = variable;
        }
        
        if (context->lastVariable) {
            context->lastVariable->nextVariable = variable;
        }

        context->lastVariable = variable;

        return variable;
    }
}

CATTO_FN_PREFIX catto_Variable* catto_setVariable(catto_Context* context, const catto_Char* name, catto_TypedValue value) {
    return catto_setVariableScoped(context, name, value, CATTO_TRUE);
}

CATTO_FN_PREFIX catto_Procedure* catto_getProcedure(catto_Context* context, const catto_Char* name) {
    catto_Procedure* currentProcedure = context->firstProcedure;

    while (currentProcedure) {
        if (catto_stringsEqualCaseInsensitive(currentProcedure->name, name)) {
            return currentProcedure;
        }

        currentProcedure = currentProcedure->nextProcedure;
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_Procedure* catto_createProcedure(catto_Context* context, const catto_Char* name) {
    catto_Procedure* newProcedure = CATTO_NEW(catto_Procedure);

    newProcedure->name = catto_copyString(name);
    newProcedure->parameterNames = (catto_Char**)CATTO_MALLOC(0);
    newProcedure->parameterCount = 0;
    newProcedure->astNode = CATTO_NULL;
    newProcedure->nextProcedure = CATTO_NULL;

    if (!context->firstProcedure) {
        context->firstProcedure = newProcedure;
    }

    if (context->lastProcedure) {
        context->lastProcedure->nextProcedure = newProcedure;
    }

    context->lastProcedure = newProcedure;

    return newProcedure;
}

CATTO_FN_PREFIX void catto_assignValue(catto_Context* context, catto_AstNode* astNode, catto_TypedValue value) {
    if (!astNode) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;

        return;
    }

    if (astNode->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;

        return;
    }

    catto_Char* name = astNode->value.asExpressionLeaf.subjectVariable;

    if (!name) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;

        return;
    }

    catto_AstNode* indexAstNode = astNode->value.asExpressionLeaf.index;

    if (indexAstNode) {
        catto_Int index = (catto_Int)catto_asNumber(catto_evalExpression(context, indexAstNode));
        catto_TypedValue variableValue = *catto_getVariable(context, name);

        if (variableValue.type != CATTO_DATA_TYPE_LIST) {
            context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

            return;
        }

        while (index < 0) {
            index += variableValue.value.asList->length;
        }

        catto_setListItem(context, variableValue.value.asList, index, catto_copyTypedValue(value));

        return;
    }

    catto_setVariable(context, name, value);
}

CATTO_FN_PREFIX catto_Bool catto_hasNextArg(catto_Context* context) {
    return !!context->nextParsedArgument;
}

CATTO_FN_PREFIX catto_TypedValue catto_evalExpression(catto_Context* context, catto_AstNode* astNode) {
    const catto_TypedValue DEFAULT_RETURN_VALUE = (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_NUMBER,
        .value = {.asNumber = 0}
    };

    if (!astNode) {
        return DEFAULT_RETURN_VALUE;
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        if (astNode->value.asExpressionLeaf.value) {
            return *(astNode->value.asExpressionLeaf.value);
        }

        catto_Char* subjectVariable = astNode->value.asExpressionLeaf.subjectVariable;
        catto_AstNode* firstArgument = astNode->value.asExpressionLeaf.firstArgument;

        if (subjectVariable) {
            catto_Char* untypedSubjectVariable = catto_copyString(subjectVariable);
            catto_DataType type = (catto_DataType)catto_removeTypeFromVariableName(untypedSubjectVariable);
            catto_TypedValue* variableValuePtr = catto_getVariable(context, untypedSubjectVariable);

            CATTO_FREE(untypedSubjectVariable);

            if (variableValuePtr) {
                catto_TypedValue variableValue = *variableValuePtr;
                catto_AstNode* indexAstNode = astNode->value.asExpressionLeaf.index;

                if (indexAstNode) {
                    catto_Int index = (catto_Int)catto_asNumber(catto_evalExpression(context, indexAstNode));

                    if (variableValue.type != CATTO_DATA_TYPE_LIST) {
                        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

                        return DEFAULT_RETURN_VALUE;
                    }

                    while (index < 0) {
                        index += variableValue.value.asList->length;
                    }

                    variableValue = catto_getListItem(variableValue.value.asList, index);
                }

                if (variableValue.type == CATTO_DATA_TYPE_FUNCTION) {
                    catto_AstNode* stashedFirstParsedArgument = context->firstParsedArgument;
                    catto_AstNode* stashedNextParsedArgument = context->nextParsedArgument;

                    context->firstParsedArgument = firstArgument;
                    context->nextParsedArgument = firstArgument;

                    variableValue = variableValue.value.asFunction(context, type);

                    context->firstParsedArgument = stashedFirstParsedArgument;
                    context->nextParsedArgument = stashedNextParsedArgument;

                    catto_addTypedValueToGc(context, variableValue);
                } else if (firstArgument) {
                    context->errorState = CATTO_ERROR_STATE_NOT_A_FUNCTION;

                    return DEFAULT_RETURN_VALUE;
                }

                if (type == CATTO_DATA_TYPE_NULL) {
                    return variableValue;
                }

                catto_TypedValue castedValue = catto_castTypedValue(variableValue, type);

                catto_addTypedValueToGc(context, castedValue);

                return castedValue;
            } else if (firstArgument) {
                context->errorState = CATTO_ERROR_STATE_NOT_A_FUNCTION;

                return DEFAULT_RETURN_VALUE;
            }
        }
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_UNARY_EXPRESSION) {
        catto_Char* operatorValue = astNode->value.asUnaryExpression.operatorValue;
        catto_Count i = 0;

        while (catto_operatorMappings[i].operatorValue) {
            catto_OperatorMapping currentOperatorMapping = catto_operatorMappings[i];

            if (catto_stringsEqualCaseInsensitive(operatorValue, currentOperatorMapping.operatorValue)) {
                catto_UnaryOperatorFunction function = currentOperatorMapping.unaryFunction;

                if (function) {
                    return function(context, catto_evalExpression(context, astNode->value.asUnaryExpression.child));
                }

                return DEFAULT_RETURN_VALUE;
            }

            i++;
        }
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_BINARY_EXPRESSION) {
        catto_AstNode* currentChild = astNode->value.asBinaryExpression.firstChild;
        catto_TypedValue currentValue = catto_evalExpression(context, currentChild);
        catto_Count i = 0;

        currentChild = currentChild->nextAstNode;

        while (currentChild) {
            catto_Char* operatorValue = astNode->value.asBinaryExpression.operatorValues[i++];
            catto_Bool foundOperatorMapping = CATTO_FALSE;
            catto_Count j = 0;

            while (catto_operatorMappings[j].operatorValue) {
                catto_OperatorMapping currentOperatorMapping = catto_operatorMappings[j];

                if (catto_stringsEqualCaseInsensitive(operatorValue, currentOperatorMapping.operatorValue)) {
                    catto_BinaryOperatorFunction function = currentOperatorMapping.binaryFunction;

                    if (function) {
                        currentValue = function(context, currentValue, catto_evalExpression(context, currentChild));
                        foundOperatorMapping = CATTO_TRUE;
                        break;
                    }

                    return DEFAULT_RETURN_VALUE;
                }

                j++;
            }

            if (!foundOperatorMapping) {
                return DEFAULT_RETURN_VALUE;
            }

            currentChild = currentChild->nextAstNode;
        }

        return currentValue;
    }

    return DEFAULT_RETURN_VALUE;
}

CATTO_FN_PREFIX catto_Bool catto_hasAppendFlag(catto_AstNode* astNode) {
    if (astNode->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        return astNode->value.asExpressionLeaf.appendFlag;
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_UNARY_EXPRESSION) {
        return catto_hasAppendFlag(astNode->value.asUnaryExpression.child);
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_BINARY_EXPRESSION) {
        catto_AstNode* currentChild = astNode->value.asBinaryExpression.firstChild;

        while (currentChild) {
            if (catto_hasAppendFlag(currentChild)) {
                return CATTO_TRUE;
            }

            currentChild = currentChild->nextAstNode;
        }
    }

    return CATTO_FALSE;
}

CATTO_FN_PREFIX catto_AstNode* catto_getNextArg(catto_Context* context) {
    catto_AstNode* currentArgument = context->nextParsedArgument;

    context->nextParsedArgument = currentArgument ? currentArgument->nextAstNode : CATTO_NULL;

    return currentArgument;
}

CATTO_FN_PREFIX catto_TypedValue catto_evalNextArg(catto_Context* context) {
    return catto_evalExpression(context, catto_getNextArg(context));
}

CATTO_FN_PREFIX catto_Bool catto_step(catto_Context* context) {
    if (!context->nextParsedStatement) {
        return CATTO_FALSE;
    }

    catto_AstNode* currentStatement = context->nextParsedStatement;

    context->currentParsedStatement = currentStatement;
    context->nextParsedStatement = currentStatement->nextAstNode;
    context->firstParsedArgument = currentStatement->value.asStatement.firstArgument;
    context->nextParsedArgument = context->firstParsedArgument;

    if (currentStatement->value.asStatement.lineNumber > 0) {
        context->subjectLineNumber = currentStatement->value.asStatement.lineNumber;
    }

    catto_gc(context);

    switch (currentStatement->type) {
        case CATTO_AST_NODE_TYPE_NOOP:
            break;

        case CATTO_AST_NODE_TYPE_COMMAND_STATEMENT:
        {
            catto_CommandHandler* commandHandler = currentStatement->value.asStatement.attributes.asCommandHandler;

            if (!commandHandler) {
                context->errorState = CATTO_ERROR_STATE_UNKNOWN_PROCEDURE;
                return CATTO_FALSE;
            }

            catto_CommandHandlerFunction function = commandHandler->function;

            if (!function) {
                return CATTO_FALSE;
            }

            function(context);

            break;
        }

        case CATTO_AST_NODE_TYPE_PROCEDURE_STATEMENT:
        {
            catto_Procedure* procedure = catto_getProcedure(context, currentStatement->value.asStatement.attributes.asProcedure.name);

            if (!procedure) {
                context->errorState = CATTO_ERROR_STATE_UNKNOWN_PROCEDURE;
                return CATTO_FALSE;
            }

            for (catto_Count i = 0; i < procedure->parameterCount; i++) {
                catto_AstNode* argument = catto_getNextArg(context);
                catto_TypedValue argumentValue = catto_evalExpression(context, argument);

                context->statementStackCount++;

                catto_Variable* variable = catto_setVariableScoped(context, procedure->parameterNames[i], argumentValue, CATTO_FALSE);

                variable->argumentReference = argument;

                context->statementStackCount--;
            }

            catto_pushOntoStatementStack(context, context->nextParsedStatement);

            context->nextParsedStatement = procedure->astNode;

            break;
        }

        case CATTO_AST_NODE_TYPE_ASSIGNMENT_STATEMENT:
        {
            catto_Char* variableName = currentStatement->value.asStatement.attributes.asAssignee.subjectVariable;
            catto_AstNode* indexAstNode = currentStatement->value.asStatement.attributes.asAssignee.index;
            catto_TypedValue value = catto_evalExpression(context, currentStatement->value.asStatement.firstArgument);

            if (indexAstNode) {
                catto_Int index = (catto_Int)catto_asNumber(catto_evalExpression(context, indexAstNode));
                catto_TypedValue variableValue = *catto_getVariable(context, variableName);

                if (variableValue.type != CATTO_DATA_TYPE_LIST) {
                    context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

                    return CATTO_FALSE;
                }

                while (index < 0) {
                    index += variableValue.value.asList->length;
                }

                catto_setListItem(context, variableValue.value.asList, index, value);

                break;
            }

            catto_setVariable(context, variableName, value);

            break;
        }

        default:
            return CATTO_FALSE;
    }

    return context->nextParsedStatement && context->errorState == CATTO_ERROR_STATE_NONE;
}

CATTO_FN_PREFIX void catto_goto(catto_Context* context, catto_Count lineNumber) {
    catto_AstNode* currentStatement = context->firstParsedStatement;

    while (currentStatement) {
        if (currentStatement->value.asStatement.lineNumber >= lineNumber) {
            break;
        }

        currentStatement = currentStatement->nextAstNode;
    }

    context->nextParsedStatement = currentStatement;
}

CATTO_FN_PREFIX void catto_pushOntoStatementStack(catto_Context* context, catto_AstNode* statement) {
    context->statementStack = (catto_AstNode**)CATTO_REALLOC(context->statementStack, (++context->statementStackCount) * sizeof(catto_AstNode**));

    context->statementStack[context->statementStackCount - 1] = statement;
}

CATTO_FN_PREFIX void catto_removeScopedVariables(catto_Context* context) {
    if (context->statementStackCount == 0) {
        return;
    }

    catto_Variable* variable = context->firstVariable;
    catto_Variable* previousVariable = CATTO_NULL;

    while (variable) {
        catto_Variable* nextVariable = variable->nextVariable;

        if (variable->scope >= context->statementStackCount) {
            if (previousVariable) {
                previousVariable->nextVariable = nextVariable;
            }
            
            if (variable == context->firstVariable) {
                context->firstVariable = nextVariable;
            }

            if (variable == context->lastVariable) {
                context->lastVariable = previousVariable;
            }

            catto_addTypedValueToGc(context, variable->value);

            CATTO_FREE(variable->name);
            CATTO_FREE(variable);
        } else {
            previousVariable = variable;
        }

        variable = nextVariable;
    }
}

CATTO_FN_PREFIX catto_AstNode* catto_popFromStatementStack(catto_Context* context) {
    if (context->statementStackCount == 0) {
        return CATTO_NULL;
    }

    catto_removeScopedVariables(context);

    catto_Count scope = context->statementStackCount;
    catto_AstNode* lastStatement = context->statementStack[context->statementStackCount - 1];

    context->statementStack = (catto_AstNode**)CATTO_REALLOC(context->statementStack, (--context->statementStackCount) * sizeof(catto_AstNode**));

    return lastStatement;
}

CATTO_FN_PREFIX void catto_load(catto_Context* context, const catto_Char* code) {
    context->errorState = CATTO_ERROR_STATE_NONE;
    context->subjectLineNumber = 0;

    catto_Token* firstToken = catto_tokenise(context, code);
    catto_Token* currentToken = firstToken;

    while (currentToken) {
        if (currentToken->type == CATTO_TOKEN_TYPE_LINE_NUMBER) {
            context->subjectLineNumber = currentToken->value.asLineNumber;
        }

        if (currentToken->type == CATTO_TOKEN_TYPE_SYNTAX_ERROR) {
            context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;

            catto_freeTokens(firstToken);

            return;
        }

        currentToken = currentToken->nextToken;
    }

    catto_AstNode* firstAstNode = catto_parse(firstToken);
    catto_AstNode* currentAstNode = firstAstNode;

    if (context->firstParsedStatement) {
        catto_freeAstNodes(context->firstParsedStatement);

        context->firstParsedStatement = CATTO_NULL;
        context->nextParsedStatement = CATTO_NULL;
    }

    context->subjectLineNumber = 0;

    while (currentAstNode) {
        if (
            currentAstNode->type == CATTO_AST_NODE_TYPE_SYNTAX_ERROR ||
            currentAstNode->type == CATTO_AST_NODE_TYPE_COMMAND_STATEMENT ||
            currentAstNode->type == CATTO_AST_NODE_TYPE_ASSIGNMENT_STATEMENT
        ) {
            catto_Count lineNumber = currentAstNode->value.asStatement.lineNumber;

            if (lineNumber > 0) {
                context->subjectLineNumber = lineNumber;
            }
        }

        if (currentAstNode->type == CATTO_AST_NODE_TYPE_SYNTAX_ERROR) {
            context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;

            catto_freeTokens(firstToken);

            return;
        }

        currentAstNode = currentAstNode->nextAstNode;
    }

    context->subjectLineNumber = 0;
    context->firstParsedStatement = firstAstNode;
    context->nextParsedStatement = firstAstNode;
    context->statementStack = (catto_AstNode**)CATTO_REALLOC(context->statementStack, 0);
    context->statementStackCount = 0;

    catto_removeScopedVariables(context);
    catto_freeTokens(firstToken);
}

CATTO_FN_PREFIX void catto_run(catto_Context* context) {
    while (catto_step(context)) {}
}

// src/numbers.h

#ifdef CATTO_USE_64_BIT
    #define CATTO_LOG2_EXPONENT_SHIFT 52
    #define CATTO_LOG2_EXPONENT_BIAS 1023
    #define CATTO_LOG2_EXPONENT_MASK 0x7FF
    #define CATTO_LOG2_MANTISSA_MASK 0xFFFFFFFFFFFFF
#else
    #define CATTO_LOG2_EXPONENT_SHIFT 23
    #define CATTO_LOG2_EXPONENT_BIAS 127
    #define CATTO_LOG2_EXPONENT_MASK 0xFF
    #define CATTO_LOG2_MANTISSA_MASK 0x7FFFFF
#endif

CATTO_FN_PREFIX catto_Float catto_log2(catto_Float value) {
    if (value <= 0) {
        return CATTO_NAN;
    }

    if (value < 1) {
        return -catto_log2(1 / value);
    }

    union {
        catto_Float asFloat;
        catto_Count asCount;
    } converter;
    
    converter.asFloat = value;

   catto_Count exponent = (converter.asCount >> CATTO_LOG2_EXPONENT_SHIFT) & CATTO_LOG2_EXPONENT_MASK;
   catto_Count mantissa = converter.asCount & CATTO_LOG2_MANTISSA_MASK;
    
    // Exponent is stored as biased, so subtract that bias
    catto_Count biasedExponent = exponent - CATTO_LOG2_EXPONENT_BIAS;

    catto_Float mantissaValue = ((catto_Float)mantissa / (1ULL << CATTO_LOG2_EXPONENT_SHIFT));
    catto_Float mantissaResult = 0;
    catto_Float mantissaPartResult = mantissaValue;

    for (catto_Count i = 1; i <= CATTO_LOG2_ITERATIONS; i++) {
        catto_Float coefficient = (i % 2 == 1) ? 1 : -1;
        
        mantissaResult += coefficient * mantissaPartResult / i;
        mantissaPartResult *= mantissaValue;
    }

    mantissaResult *= 1.4426950408889634;
    
    return biasedExponent + mantissaResult;
}

CATTO_FN_PREFIX catto_Float catto_log(catto_Float value) {
    return catto_roundToPrecision(catto_log2(value) / 3.3219280948873623, 4);
}

CATTO_FN_PREFIX catto_Float catto_ln(catto_Float value) {
    return catto_log2(value) / 1.4426950408889634;
}

CATTO_FN_PREFIX catto_Float catto_exp2(catto_Float value) {
    value *= 0.6931471805599453;

    catto_Float partResult = 1;
    catto_Float result = 1;

    for (catto_Count i = 1; i <= CATTO_EXP2_ITERATIONS; i++) {
        partResult *= value / i;
        result += partResult;
    }

    return result;
}

CATTO_FN_PREFIX catto_Float catto_power(catto_Float base, catto_Float power) {
    if (power == 0) {
        return 1;
    }

    if (power < 0) {
        base = 1 / base;
        power *= -1;
    }

    if (power != (catto_Int)power) {
        return catto_exp2(power * catto_log2(base));
    }   

    catto_Float result = base;

    while (power > 1) {
        result *= base;
        power--;
    }

    return result;
}

// @source https://stackoverflow.com/a/49991852
CATTO_FN_PREFIX catto_Float catto_sqrt(catto_Float value) {
    catto_Float result = 1;

    if (value < 0) {
        return CATTO_NAN;
    }

    for (catto_Count i = 1; i <= CATTO_SQRT_ITERATIONS; i++) {
        result -= ((result * result) - value) / (2 * result);
    }

    return result;
}

CATTO_FN_PREFIX catto_Float catto_fromRadians(catto_Float value, catto_TrigMode trigMode) {
    switch (trigMode) {
        case CATTO_TRIG_MODE_RADIANS: return value;
        case CATTO_TRIG_MODE_DEGREES: return value / (CATTO_PI / 180);
        case CATTO_TRIG_MODE_GRADIANS: return value / (CATTO_PI / 200);
        case CATTO_TRIG_MODE_TURNS: return value / (2 * CATTO_PI);
    }

    return value;
}

CATTO_FN_PREFIX catto_Float catto_toRadians(catto_Float value, catto_TrigMode trigMode) {
    switch (trigMode) {
        case CATTO_TRIG_MODE_RADIANS: return value;
        case CATTO_TRIG_MODE_DEGREES: return value * (CATTO_PI / 180);
        case CATTO_TRIG_MODE_GRADIANS: return value * (CATTO_PI / 200);
        case CATTO_TRIG_MODE_TURNS: return value * (2 * CATTO_PI);
    }

    return value;
}

CATTO_FN_PREFIX catto_Float catto_floatMod(catto_Float a, catto_Float b) {
    catto_Float divisionResult = a / b;
    catto_Float flooredResult = (catto_Int)(divisionResult < 0 ? divisionResult - 1 : divisionResult);

    return a - (flooredResult * b);
}

CATTO_FN_PREFIX catto_Float catto_sin(catto_Float value) {
    return catto_cos(value - (CATTO_PI / 2));
}

// @source https://stackoverflow.com/a/2284969
CATTO_FN_PREFIX catto_Float catto_cos(catto_Float value) {
    catto_Float partResult = 1;
    catto_Float result = 1;

    value = catto_floatMod(value, 2 * CATTO_PI);

    for (catto_Count i = 1; i <= CATTO_COS_ITERATIONS; i++) {
        partResult = (-partResult * value * value) / (((2 * i) - 1) * 2 * i);
        result += partResult;
    }

    return catto_roundToPrecision(result, 14);
}

CATTO_FN_PREFIX catto_Float catto_tan(catto_Float value) {
    return catto_sin(value) / catto_cos(value);
}

CATTO_FN_PREFIX catto_Float catto_asin(catto_Float value) {
    if (value < -1 || value > 1) {
        return CATTO_NAN;
    }

    return catto_atan(value / catto_sqrt(1 - (value * value)));
}

CATTO_FN_PREFIX catto_Float catto_acos(catto_Float value) {
    if (value < -1 || value > 1) {
        return CATTO_NAN;
    }

    if (value == -1) {
        return CATTO_PI;
    }

    catto_Float result = catto_atan(catto_sqrt(1 - (value * value)) / value);

    return result < 0 ? result + CATTO_PI : result;
}

CATTO_FN_PREFIX catto_Float catto_atan(catto_Float value) {
    const catto_Float results[] = {
        0,
        0.01745506492, 0.03492076949, 0.05240777928, 0.06992681194,
        0.08748866352, 0.10510423526, 0.12278456090, 0.14054083470,
        0.15838444032, 0.17632698070, 0.19438030913, 0.21255656167,
        0.23086819112, 0.24932800284, 0.26794919243, 0.28674538575,
        0.30573068145, 0.32491969623, 0.34432761328, 0.36397023426,
        0.38386403503, 0.40402622583, 0.42447481620, 0.44522868530,
        0.46630765815, 0.48773258856, 0.50952544949, 0.53170943166,
        0.55430905145, 0.57735026918, 0.60086061902, 0.62486935190,
        0.64940759319, 0.67450851684, 0.70020753820, 0.72654252800,
        0.75355405010, 0.78128562650, 0.80978403319, 0.83909963117,
        0.86928673781, 0.90040404429, 0.93251508613, 0.96568877480,
        0.99999999999, 1.03553031379, 1.07236871002, 1.11061251482,
        1.15036840722, 1.19175359259, 1.23489715653, 1.27994163219,
        1.32704482162, 1.37638192047, 1.42814800674, 1.48256096851,
        1.53986496381, 1.60033452904, 1.66427948235, 1.73205080756,
        1.80404775527, 1.88072646534, 1.96261050550, 2.05030384157,
        2.14450692050, 2.24603677390, 2.35585236582, 2.47508685341,
        2.60508906469, 2.74747741945, 2.90421087767, 3.07768353717,
        3.27085261848, 3.48741444384, 3.73205080756, 4.01078093353,
        4.33147587428, 4.70463010947, 5.14455401597, 5.67128181961,
        6.31375151467, 7.11536972238, 8.14434642797, 9.51436445422,
        11.4300523027, 14.3006662567, 19.0811366877, 28.6362532829,
        57.2899616307, 171.869884892, 1.0 / 0.0
    };

    const catto_Count resultCount = sizeof(results) / sizeof(results[0]);

    catto_Float result = 90;
    catto_Bool negative = value < 0;

    if (negative) {
        value *= -1;
    }

    for (catto_Count i = 0; i < resultCount - 1; i++) {
        if (value >= results[i] - CATTO_EPSILON && value < results[i + 1] + CATTO_EPSILON) {
            result = i + ((value - results[i]) / (results[i + 1] - results[i]));
            break;
        }
    }

    result = catto_toRadians(catto_roundToPrecision(result, 10), CATTO_TRIG_MODE_DEGREES);

    if (negative) {
        result *= -1;
    }

    return result;
}

CATTO_FN_PREFIX catto_Float catto_roundToPrecision(catto_Float number, catto_Count precision) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    catto_Int integralPart = number;
    catto_Count integralDigits = 0;

    while (integralPart > 0) {
        integralPart /= 10;
        integralDigits++;
    }

    precision -= integralDigits;

    if (precision < 0) {
        precision = 0;
    }

    catto_Int multiplier = catto_power(10, precision);

    number += 0.5 * catto_power(10, -(catto_Int)precision);

    if (isNegative) {
        number *= -1;
    }

    return (catto_Float)((catto_Int)(number * multiplier)) / multiplier;
}

CATTO_FN_PREFIX catto_Char* catto_numberToString(catto_Float number) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    if (number != number) {
        return catto_copyString("NaN");
    }

    if (number == CATTO_INFINITY) {
        return catto_copyString(isNegative ? "-Infinity" : "Infinity");
    }

    catto_Int exponent = 0;
    catto_Char* string = catto_copyString("");
    catto_Count precisionLeft = CATTO_MAX_PRECISION;

    if (number > 0) {
        if (number < catto_power(10, -CATTO_MAX_PRECISION + 1)) {
            while (number < 1 - catto_power(10, -CATTO_MAX_PRECISION)) {
                number *= 10;
                exponent--;
            }
        }

        if (number > catto_power(10, CATTO_MAX_PRECISION - 1)) {
            while (number > 10 + catto_power(10, -CATTO_MAX_PRECISION)) {
                number /= 10;
                exponent++;
            }
        }
    }

    number = catto_roundToPrecision(number, CATTO_MAX_PRECISION);

    catto_Int integralPart = number;

    number += 0.1 * catto_power(10, -(catto_Int)precisionLeft);
    number -= integralPart; // Now fractional part

    do {
        string = catto_appendCharToString(string, (catto_Char)('0' + (integralPart % 10)));

        integralPart /= 10;
        precisionLeft--;
    } while (integralPart > 0);

    if (isNegative) {
        string = catto_appendCharToString(string, '-');
    }

    catto_reverseString(string);

    catto_Count trailingZeroes = 0;
    catto_Bool anyDigitsInFractionalPart = CATTO_FALSE;

    if (number > CATTO_EPSILON && precisionLeft > 0) {
        string = catto_appendCharToString(string, '.');

        while (number > CATTO_EPSILON && precisionLeft > 0) {
            number *= 10;

            catto_Char digit = number;

            if (digit == 0) {
                trailingZeroes++;
            } else {
                trailingZeroes = 0;
                anyDigitsInFractionalPart = CATTO_TRUE;
            }

            string = catto_appendCharToString(string, (catto_Char)('0' + digit));

            number -= digit;
            precisionLeft--;
        }
    }

    if (trailingZeroes > 0) {
        if (!anyDigitsInFractionalPart) {
            trailingZeroes++;
        }

        catto_Count newStringLength = catto_stringLength(string) - trailingZeroes;

        string = (catto_Char*)CATTO_REALLOC(string, newStringLength + 1);
        string[newStringLength] = '\0';
    }

    if (exponent != 0) {
        string = catto_appendCharToString(string, 'E');

        if (exponent > 0) {
            string = catto_appendCharToString(string, '+');
        }

        catto_Char* exponentString = catto_numberToString(exponent);

        string = catto_appendToString(string, exponentString);

        CATTO_FREE(exponentString);
    }

    return string;
}

CATTO_FN_PREFIX catto_Char* catto_numberToBaseString(catto_Float number, catto_Count base) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    if (number != number) {
        return catto_copyString("NaN");
    }

    if (number == CATTO_INFINITY) {
        return catto_copyString(isNegative ? "-Infinity" : "Infinity");
    }

    catto_Char* string = catto_copyString("");

    do {
        catto_Char digit = (catto_Int)number % base;

        if (digit < 10) {
            digit += '0';
        } else if (digit < 16) {
            digit += 'a' - 10;
        } else {
            digit = '?';
        }

        string = catto_appendCharToString(string, digit);

        number /= base;
    } while (number >= 1);

    if (isNegative) {
        string = catto_appendCharToString(string, '-');
    }

    catto_reverseString(string);

    return string;
}

// src/strings.h

#ifndef CATTO_STRINGS_H_
#define CATTO_STRINGS_H_

CATTO_FN_PREFIX catto_Count catto_stringLength(const catto_Char* string) {
    catto_Count length = 0;

    while (string[length] != '\0') {
        length++;
    }

    return length;
}

CATTO_FN_PREFIX catto_Bool _catto_charsEqual(catto_Char a, catto_Char b, catto_Bool caseInsensitive) {
    if (caseInsensitive) {
        if (a >= 'A' && a <= 'Z') {
            a += 32;
        }

        if (b >= 'A' && b <= 'Z') {
            b += 32;
        }
    }

    return a == b;
}

CATTO_FN_PREFIX catto_Bool _catto_stringsEqual(const catto_Char* a, const catto_Char* b, catto_Bool caseInsensitive) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (_catto_charsEqual(a[i], b[i], caseInsensitive)) {
        if (a[i] == '\0') {
            if (b[i] == '\0') {
                return CATTO_TRUE;
            }

            return CATTO_FALSE;
        }

        if (b[i] == '\0') {
            return CATTO_FALSE;
        }

        i++;
    }

    return CATTO_FALSE;
}

CATTO_FN_PREFIX catto_Bool catto_stringsEqual(const catto_Char* a, const catto_Char* b) {
    return _catto_stringsEqual(a, b, CATTO_FALSE);
}

CATTO_FN_PREFIX catto_Bool catto_stringsEqualCaseInsensitive(const catto_Char* a, const catto_Char* b) {
    return _catto_stringsEqual(a, b, CATTO_TRUE);
}

CATTO_FN_PREFIX catto_Char* catto_copyString(const catto_Char* string) {
    catto_Count length = catto_stringLength(string);
    catto_Char* newString = (catto_Char*)CATTO_MALLOC(length + 1);

    for (catto_Count i = 0; i < length; i++) {
        newString[i] = string[i];
    }

    newString[length] = '\0';

    return newString;
}

CATTO_FN_PREFIX catto_Char* catto_appendCharToString(catto_Char* string, catto_Char character) {
    catto_Count length = catto_stringLength(string);

    string = (catto_Char*)CATTO_REALLOC(string, length + 2);
    string[length] = character;
    string[length + 1] = '\0';

    return string;
}

CATTO_FN_PREFIX catto_Char* catto_appendToString(catto_Char* a, const catto_Char* b) {
    catto_Count aLength = catto_stringLength(a);
    catto_Count bLength = catto_stringLength(b);

    a = (catto_Char*)CATTO_REALLOC(a, aLength + bLength + 1);
    a[aLength + bLength] = '\0';

    for (catto_Count i = 0; i < bLength; i++) {
        a[aLength + i] = b[i];
    }

    return a;
}

CATTO_FN_PREFIX catto_Char* catto_reverseString(catto_Char* string) {
    catto_Char* tempString = catto_copyString(string);
    catto_Count stringLength = catto_stringLength(string);

    for (catto_Count i = 0; i < stringLength; i++) {
        string[stringLength - 1 - i] = tempString[i];
    }

    CATTO_FREE(tempString);

    return string;
}

CATTO_FN_PREFIX catto_Bool _catto_stringStartsWith(const catto_Char* a, const catto_Char* b, catto_Bool caseInsensitive) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (_catto_charsEqual(a[i], b[i], caseInsensitive)) {
        if (a[i] == '\0') {
            if (b[i] == '\0') {
                return CATTO_TRUE;
            }

            return CATTO_FALSE;
        }

        i++;
    }

    if (b[i] == '\0') {
        return CATTO_TRUE;
    }

    return CATTO_FALSE;
}

CATTO_FN_PREFIX catto_Bool catto_stringStartsWith(const catto_Char* a, const catto_Char* b) {
    return _catto_stringStartsWith(a, b, CATTO_FALSE);
}

CATTO_FN_PREFIX catto_Bool catto_stringStartsWithCaseInsensitive(const catto_Char* a, const catto_Char* b) {
    return _catto_stringStartsWith(a, b, CATTO_TRUE);
}

CATTO_FN_PREFIX catto_Float catto_unsignedStringToBaseNumber(const catto_Char* string, catto_Count base, catto_Count* charactersEaten) {
    *charactersEaten = 0;

    catto_Count i = 0;
    catto_Float result = 0;

    while (string[i] != '\0') {
        if (
            (string[i] == '0' || string[i] == '1') ||
            (base >= 8 && string[i] >= '2' && string[i] <= '7') ||
            (base >= 10 && string[i] >= '8' && string[i] <= '9')
        ) {
            result *= base;
            result += string[i] - '0';
        } else if (base >= 16 && string[i] >= 'A' && string[i] <= 'F') {
            result *= base;
            result += string[i] - 'A' + 0xA;
        } else if (base >= 16 && string[i] >= 'a' && string[i] <= 'f') {
            result *= base;
            result += string[i] - 'a' + 0xA;
        } else {
            break;
        }

        i++;
    }

    *charactersEaten = i;

    return result;
}

// @source https://stackoverflow.com/a/4392789
CATTO_FN_PREFIX catto_Float catto_unsignedStringToNumber(const catto_Char* string, catto_Count* charactersEaten) {
    *charactersEaten = 0;

    catto_Count i = 0;
    catto_Float result = 0;
    catto_Float factor = 1;
    catto_Float exponent = 0;
    catto_Float exponentIsNegative = CATTO_FALSE;
    catto_Bool afterPoint = CATTO_FALSE;
    catto_Bool hadDigit = CATTO_FALSE;
    catto_Bool afterExponentMark = CATTO_FALSE;
    catto_Bool afterExponentSign = CATTO_FALSE;

    if (string[0] == '0') {
        switch (string[1]) {
            case 'b': case 'B': result = catto_unsignedStringToBaseNumber(string + 2, 2, charactersEaten); break;
            case 'o': case 'O': result = catto_unsignedStringToBaseNumber(string + 2, 8, charactersEaten); break;
            case 'x': case 'X': result = catto_unsignedStringToBaseNumber(string + 2, 16, charactersEaten); break;

            default: break;
        }

        if (*charactersEaten > 0) {
            *charactersEaten += 2;

            return result;
        }
    }

    while (string[i] != '\0') {
        catto_Char character = string[i];

        if (character == '.' && !afterPoint) {
            afterPoint = CATTO_TRUE;
        } else if (
            (character == 'e' || character == 'E') &&
            !afterExponentMark && hadDigit
        ) {
            afterExponentMark = CATTO_TRUE;
            hadDigit = CATTO_FALSE;
        } else if (
            (character == '+' || character == '-') &&
            afterExponentMark && !afterExponentSign && !hadDigit
        ) {
            exponentIsNegative = character == '-';
            afterExponentSign = CATTO_TRUE;
        } else if (character >= '0' && character <= '9') {
            catto_Int digit = character - '0';

            if (afterExponentMark) {
                exponent = (exponent * 10.0) + digit;
            } else {
                if (afterPoint) {
                    factor /= 10.0;
                }

                result = (result * 10.0) + digit;
            }

            hadDigit = CATTO_TRUE;
        } else {
            break;
        }

        i++;
    }

    if (!afterExponentMark) {
        exponent = 1;
    }

    if (!exponentIsNegative) {
        if (exponent == 1) {
            // Do nothing
        } else if (exponent == 0) {
            result = 1;
        } else {
            for (catto_Count i = 0; i < exponent; i++) {
                result *= 10.0;
            }
        }
    } else {
        for (catto_Count i = 0; i < exponent; i++) {
            result /= 10.0;
        }
    }

    *charactersEaten = i;

    return result * factor;
}

CATTO_FN_PREFIX catto_Float catto_stringToNumber(const catto_Char* string, catto_Count* charactersEaten) {
    catto_Bool ateSign = CATTO_FALSE;
    catto_Bool negate = CATTO_FALSE;

    if (string[0] == '+' || string[0] == '-') {
        negate = string[0] == '-';
        ateSign = CATTO_TRUE;
        string += 1;
    }

    catto_Float result = catto_unsignedStringToNumber(string, charactersEaten);

    if (ateSign) {
        (*charactersEaten)++;
    }

    if (negate) {
        result *= -1;
    }

    return result;
}

CATTO_FN_PREFIX catto_Float catto_stringToBaseNumber(const catto_Char* string, catto_Count base, catto_Count* charactersEaten) {
    catto_Bool ateSign = CATTO_FALSE;
    catto_Bool negate = CATTO_FALSE;

    if (string[0] == '+' || string[0] == '-') {
        negate = string[0] == '-';
        ateSign = CATTO_TRUE;
        string += 1;
    }

    catto_Float result = catto_unsignedStringToBaseNumber(string, base, charactersEaten);

    if (ateSign) {
        (*charactersEaten)++;
    }

    if (negate) {
        result *= -1;
    }

    return result;
}

#endif

// src/lists.h

CATTO_FN_PREFIX catto_List* catto_newList() {
    catto_List* list = CATTO_NEW(catto_List);

    list->values = (catto_TypedValue*)CATTO_MALLOC(0);
    list->length = 0;
    list->referenceCount = 0;

    return list;
}

CATTO_FN_PREFIX catto_List* catto_referenceList(catto_List* list) {
    list->referenceCount++;

    return list;
}

CATTO_FN_PREFIX void catto_dereferenceList(catto_Context* context, catto_List* list) {
    if (list->referenceCount > 0) {
        list->referenceCount--;
    }

    if (list->referenceCount == 0) {
        catto_addPointerToGc(context, CATTO_DATA_TYPE_LIST, list);
    }
}

CATTO_FN_PREFIX void catto_freeList(catto_Context* context, catto_List* list) {
    for (catto_Count i = 0; i < list->length; i++) {
        catto_addTypedValueToGc(context, list->values[i]);
    }

    CATTO_FREE(list->values);
    CATTO_FREE(list);
}

CATTO_FN_PREFIX void catto_pushOntoList(catto_List* list, catto_TypedValue value) {
    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, (++list->length) * sizeof(catto_TypedValue));
    list->values[list->length - 1] = catto_copyTypedValue(value);
}

CATTO_FN_PREFIX catto_TypedValue catto_popFromList(catto_Context* context, catto_List* list) {
    if (list->length == 0) {
        return catto_asTypedNumber(0);
    }

    catto_TypedValue value = list->values[--list->length];

    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, list->length * sizeof(catto_TypedValue));

    catto_addTypedValueToGc(context, value);

    return value;
}

CATTO_FN_PREFIX void catto_insertIntoList(catto_List* list, catto_TypedValue value, catto_Count index) {
    if (index >= list->length) {
        catto_pushOntoList(list, value);

        return;
    }

    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, (++list->length) * sizeof(catto_TypedValue));

    for (catto_Count i = list->length - 1; i > index; i--) {
        list->values[i] = list->values[i - 1];
    }

    list->values[index] = catto_copyTypedValue(value);
}

CATTO_FN_PREFIX catto_TypedValue catto_removeFromList(catto_Context* context, catto_List* list, catto_Count index) {
    if (index >= list->length - 1) {
        return catto_popFromList(context, list);
    }

    catto_TypedValue value = list->values[index];

    for (catto_Count i = index; i < list->length - 1; i++) {
        list->values[i] = list->values[i + 1];
    }

    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, (--list->length) * sizeof(catto_TypedValue));

    catto_addTypedValueToGc(context, value);

    return value;
}

CATTO_FN_PREFIX catto_TypedValue catto_getListItem(catto_List* list, catto_Count index) {
    if (index >= list->length) {
        return catto_asTypedNumber(0);
    }

    return list->values[index];
}

CATTO_FN_PREFIX void catto_setListItem(catto_Context* context, catto_List* list, catto_Count index, catto_TypedValue value) {
    if (index >= list->length) {
        while (index > 0 && list->length < index) {
            catto_pushOntoList(list, catto_asTypedNumber(0));
        }

        catto_pushOntoList(list, value);

        return;
    }

    catto_addTypedValueToGc(context, list->values[index]);

    list->values[index] = value;
}

CATTO_FN_PREFIX catto_Char* catto_listToString(catto_List* list) {
    if (list->length == 0) {
        return catto_copyString("");
    }

    catto_Char* string = catto_asString(list->values[0]);

    for (catto_Count i = 1; i < list->length; i++) {
        string = catto_appendToString(string, ", ");

        catto_Char* nextItemString = catto_asString(list->values[i]);

        string = catto_appendToString(string, nextItemString);

        CATTO_FREE(nextItemString);
    }

    return string;
}

// src/datatypes.h

CATTO_FN_PREFIX catto_Float catto_asNumber(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return value.value.asNumber;
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_Count charactersEaten = 0;

        return catto_stringToNumber(value.value.asString, &charactersEaten);
    }

    return 0;
}

CATTO_FN_PREFIX catto_TypedValue catto_asTypedNumber(catto_Float value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_NUMBER,
        .value = {.asNumber = value}
    };
}

CATTO_FN_PREFIX catto_Char* catto_asString(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return catto_numberToString(value.value.asNumber);
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        return catto_copyString(value.value.asString);
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        return catto_listToString(value.value.asList);
    }

    return catto_copyString("");
}

CATTO_FN_PREFIX catto_TypedValue catto_asTypedString(catto_Char* value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_STRING,
        .value = {.asString = catto_copyString(value)}
    };
}

CATTO_FN_PREFIX catto_Bool catto_asBool(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return value.value.asNumber != 0;
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        return catto_stringLength(value.value.asString) > 0;
    }

    return CATTO_FALSE;
}

CATTO_FN_PREFIX void catto_freeTypedValue(catto_TypedValue* valuePtr) {
    if (!valuePtr) {
        return;
    }

    if (valuePtr->type == CATTO_DATA_TYPE_STRING) {
        CATTO_FREE(valuePtr->value.asString);
    }

    CATTO_FREE(valuePtr);
}

CATTO_FN_PREFIX catto_TypedValue catto_copyTypedValue(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        value.value.asString = catto_copyString(value.value.asString);
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        catto_referenceList(value.value.asList);
    }

    return value;
}

CATTO_FN_PREFIX catto_TypedValue catto_castTypedValue(catto_TypedValue value, catto_DataType type) {
    if (type == CATTO_DATA_TYPE_NULL) {
        return catto_copyTypedValue(value);
    }

    if (type == CATTO_DATA_TYPE_NUMBER) {
        value.value.asNumber = catto_asNumber(value);
        value.type = CATTO_DATA_TYPE_NUMBER;
    }

    if (type == CATTO_DATA_TYPE_STRING) {
        value.value.asString = catto_asString(value);
        value.type = CATTO_DATA_TYPE_STRING;
    }

    return value;
}

CATTO_FN_PREFIX void catto_addTypedValueToGc(catto_Context* context, catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_addPointerToGc(context, CATTO_DATA_TYPE_STRING, value.value.asString);
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        catto_dereferenceList(context, value.value.asList);
    }
}

CATTO_FN_PREFIX void catto_removeTypedValueFromGc(catto_Context* context, catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_removePointerFromGc(context, value.value.asString);
    }
}

// src/tokeniser.h

#ifndef CATTO_TOKENISER_H_
#define CATTO_TOKENISER_H_

CATTO_FN_PREFIX catto_Token* catto_addToken(catto_TokenType type, catto_Token** currentTokenPtr) {
    catto_Token* token = CATTO_NEW(catto_Token);

    token->type = type;
    token->nextToken = CATTO_NULL;

    if (*currentTokenPtr) {
        (*currentTokenPtr)->nextToken = token;
    }

    *currentTokenPtr = token;

    return token;
}

CATTO_FN_PREFIX catto_Bool catto_onWordBoundary(const catto_Char* code, catto_Count index) {
    return !(
        (code[index] >= 'a' && code[index] <= 'z') ||
        (code[index] >= 'A' && code[index] <= 'Z') ||
        (code[index] >= '0' && code[index] <= '9') ||
        code[index] == '_'
    );
}

CATTO_FN_PREFIX catto_Token* catto_matchLineNumber(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    if (*currentTokenPtr && (*currentTokenPtr)->type != CATTO_TOKEN_TYPE_NEXT_LINE) {
        return CATTO_NULL;
    }

    catto_Count index = *indexPtr;
    catto_Count lineNumber = 0;

    while (code[index] >= '0' && code[index] <= '9') {
        lineNumber *= 10;
        lineNumber += code[index] - '0';

        index++;
    }

    if (lineNumber == 0) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_LINE_NUMBER, currentTokenPtr);

    token->value.asLineNumber = lineNumber;

    *indexPtr = index;

    return token;
}

CATTO_FN_PREFIX catto_Token* catto_matchComment(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;

    if (!(code[index] == '#' || catto_stringStartsWithCaseInsensitive(code + index, "rem"))) {
        return CATTO_NULL;
    }

    while (code[index] != '\n' && code[index] != '\0') {
        index++;
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_COMMENT, currentTokenPtr);

    *indexPtr = index;

    return token;
}

CATTO_FN_PREFIX catto_Token* catto_matchChar(catto_Char matchChar, catto_TokenType type, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    if (code[*indexPtr] != matchChar) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(type, currentTokenPtr);

    (*indexPtr)++;

    return token;
}

CATTO_FN_PREFIX catto_Token* catto_matchCommand(catto_Context* context, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_CommandHandler* currentCommandHandler = context->firstCommandHandler;

    while (currentCommandHandler) {
        if (catto_stringStartsWithCaseInsensitive(code + index, currentCommandHandler->name)) {
            catto_Count newIndex = index + catto_stringLength(currentCommandHandler->name);

            if (!context->scrawlMode && !catto_onWordBoundary(code, newIndex)) {
                currentCommandHandler = currentCommandHandler->nextCommandHandler;

                continue;
            }

            if (catto_stringsEqualCaseInsensitive(currentCommandHandler->name, "scrawl")) {
                context->scrawlMode = CATTO_TRUE;
            }

            if (catto_stringsEqualCaseInsensitive(currentCommandHandler->name, "noscrawl")) {
                context->scrawlMode = CATTO_FALSE;
            }

            catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_COMMAND, currentTokenPtr);

            token->value.asCommandHandler = currentCommandHandler;

            *indexPtr = newIndex;

            return token;
        }

        currentCommandHandler = currentCommandHandler->nextCommandHandler;
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_Token* catto_matchStrings(const catto_Char** matchStrings, catto_TokenType type, catto_Bool caseInsensitive, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_Count i = 0;

    while (matchStrings[i]) {
        const catto_Char* currentString = matchStrings[i];

        if (_catto_stringStartsWith(code + index, currentString, caseInsensitive)) {
            catto_Token* token = catto_addToken(type, currentTokenPtr);

            token->value.asConstString = currentString;

            *indexPtr = index + catto_stringLength(currentString);

            return token;
        }

        i++;
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_Token* catto_matchNumber(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count charactersEaten = 0;
    catto_Float number = catto_unsignedStringToNumber(code + *indexPtr, &charactersEaten);

    if (number == CATTO_NAN || charactersEaten == 0) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_NUMBER, currentTokenPtr);

    token->value.asNumber = number;

    *indexPtr += charactersEaten;

    return token;
}

CATTO_FN_PREFIX catto_Token* catto_matchStringLiteral(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;

    catto_Char stringOpener = code[index++];
    catto_Char* currentString = (catto_Char*)CATTO_MALLOC(8);
    catto_Count currentStringIndex = 0;

    currentString[currentStringIndex] = '\0';

    if (stringOpener != '"' && stringOpener != '\'' && stringOpener != '`') {
        CATTO_FREE(currentString);

        return CATTO_NULL;
    }

    while (CATTO_TRUE) {
        catto_Char currentChar = code[index++];

        if (currentChar == stringOpener) {
            break;
        }

        if (currentChar == '\0' || currentChar == '\n') {
            CATTO_FREE(currentString);

            return CATTO_NULL;
        }

        if (currentChar == '\\') {
            switch (code[index]) {
                case '\0':
                case '\n':
                    CATTO_FREE(currentString);

                    return CATTO_NULL;

                case 'n': currentChar = '\n'; break;
                case 'r': currentChar = '\r'; break;
                case 't': currentChar = '\t'; break;
                case 'v': currentChar = '\v'; break;
                case 'b': currentChar = '\b'; break;
                case 'f': currentChar = '\f'; break;

                default:
                    currentChar = code[index];
            }

            index++;
        }

        currentString[currentStringIndex++] = currentChar;
        currentString[currentStringIndex] = '\0';

        if ((currentStringIndex + 1) % 8 == 0) {
            currentString = (catto_Char*)CATTO_REALLOC(currentString, currentStringIndex + 9);
        }
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_STRING, currentTokenPtr);

    token->value.asString = currentString;

    *indexPtr = index;

    return token;
}

CATTO_FN_PREFIX catto_Token* catto_matchIdentifier(catto_Context* context, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_Char* currentString = (catto_Char*)CATTO_MALLOC(8);
    catto_Count currentStringIndex = 0;
    catto_Char currentChar = code[index++];

    if (!(
        (currentChar >= 'a' && currentChar <= 'z') ||
        (currentChar >= 'A' && currentChar <= 'Z') ||
        currentChar == '_'
    )) {
        CATTO_FREE(currentString);

        return CATTO_NULL;
    }

    currentString[currentStringIndex++] = currentChar;
    currentString[currentStringIndex] = 0;

    catto_Bool shouldContinue = CATTO_TRUE;

    while (shouldContinue) {
        currentChar = code[index++];

        if (currentChar == '$' || currentChar == '%') {
            shouldContinue = CATTO_FALSE;
        } else if (!(
            (currentChar >= 'a' && currentChar <= 'z') ||
            (currentChar >= 'A' && currentChar <= 'Z') ||
            (!context->scrawlMode && currentChar >= '0' && currentChar <= '9') ||
            currentChar == '_'
        )) {
            index--;

            break;
        }

        currentString[currentStringIndex++] = currentChar;
        currentString[currentStringIndex] = '\0';

        if ((currentStringIndex + 1) % 8 == 0) {
            currentString = (catto_Char*)CATTO_REALLOC(currentString, currentStringIndex + 9);
        }
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_IDENTIFIER, currentTokenPtr);

    token->value.asString = currentString;

    *indexPtr = index;

    return token;
}

CATTO_FN_PREFIX catto_Token* catto_tokenise(catto_Context* context, const catto_Char* code) {
    catto_Token* firstToken = CATTO_NULL;
    catto_Token* currentToken = CATTO_NULL;
    catto_Count index = 0;
    catto_Count length = catto_stringLength(code);

    while (index < length) {
        catto_Char currentChar = code[index];

        if (currentToken && !firstToken) {
            firstToken = currentToken;
        }

        if (code[index] == ' ') {
            index++;
 
            continue;
        }

        if (catto_matchChar('\n', CATTO_TOKEN_TYPE_NEXT_LINE, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchLineNumber(code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchComment(code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchChar(',', CATTO_TOKEN_TYPE_DELIMETER, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchChar(':', CATTO_TOKEN_TYPE_STATEMENT_DELIMETER, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchChar('(', CATTO_TOKEN_TYPE_OPENING_BRACKET, code, &index, &currentToken)) {
            continue;
        }
        
        if (catto_matchChar(')', CATTO_TOKEN_TYPE_CLOSING_BRACKET, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchChar('[', CATTO_TOKEN_TYPE_OPENING_ACCESSOR_BRACKET, code, &index, &currentToken)) {
            continue;
        }
        
        if (catto_matchChar(']', CATTO_TOKEN_TYPE_CLOSING_ACCESSOR_BRACKET, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchStrings(catto_operators, CATTO_TOKEN_TYPE_OPERATOR, CATTO_TRUE, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchCommand(context, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchStringLiteral(code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchIdentifier(context, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchNumber(code, &index, &currentToken)) {
            continue;
        }

        catto_addToken(CATTO_TOKEN_TYPE_SYNTAX_ERROR, &currentToken);

        break;
    }

    if (!firstToken) {
        return currentToken;
    }

    return firstToken;
}

CATTO_FN_PREFIX void catto_freeTokens(catto_Token* firstToken) {
    catto_Token* currentToken = firstToken;

    while (currentToken) {
        catto_Token* lastToken = currentToken;

        switch (currentToken->type) {
            case CATTO_TOKEN_TYPE_STRING:
            case CATTO_TOKEN_TYPE_IDENTIFIER:
                CATTO_FREE(currentToken->value.asString);
                break;

            default:
                break;
        }

        currentToken = currentToken->nextToken;

        CATTO_FREE(lastToken);
    }
}

CATTO_FN_PREFIX void catto_debugTokens(catto_Token* firstToken) {
    catto_Token* currentToken = firstToken;

    while (currentToken) {
        CATTO_LOG_CHAR(currentToken->type);

        currentToken = currentToken->nextToken;
    }
}

#endif

// src/parser.h

CATTO_FN_PREFIX catto_Token* catto_eat(catto_Token** currentTokenPtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Token* returnToken = *currentTokenPtr;

    *currentTokenPtr = returnToken->nextToken;
    
    return returnToken;
}

CATTO_FN_PREFIX catto_Token* catto_eatIfType(catto_Token** currentTokenPtr, catto_TokenType type) {
    if (!*currentTokenPtr || (*currentTokenPtr)->type != type) {
        return CATTO_NULL;
    }

    return catto_eat(currentTokenPtr);
}

CATTO_FN_PREFIX catto_Token* catto_eatIfKeyword(catto_Token** currentTokenPtr, const catto_Char* keyword) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    if (
        ((*currentTokenPtr)->type == CATTO_TOKEN_TYPE_COMMAND && catto_stringsEqualCaseInsensitive((*currentTokenPtr)->value.asCommandHandler->name, keyword)) ||
        ((
            (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_IDENTIFIER ||
            (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_OPERATOR
        ) && catto_stringsEqualCaseInsensitive((*currentTokenPtr)->value.asString, keyword))
    ) {
        return catto_eat(currentTokenPtr);
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_AstNode* catto_addAstNode(catto_AstNodeType type, catto_AstNode** currentAstNodePtr) {
    catto_AstNode* astNode = CATTO_NEW(catto_AstNode);

    astNode->type = type;
    astNode->nextAstNode = CATTO_NULL;

    if (*currentAstNodePtr) {
        (*currentAstNodePtr)->nextAstNode = astNode;
    }

    *currentAstNodePtr = astNode;

    return astNode;
}

CATTO_FN_PREFIX catto_AstNode* catto_createExpressionLeaf(catto_TypedValue value, catto_AstNode** currentAstNodePtr) {
    catto_TypedValue* valuePtr = CATTO_NEW(catto_TypedValue);

    valuePtr->type = value.type;
    valuePtr->value = value.value;

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_EXPRESSION_LEAF, currentAstNodePtr);

    astNode->value.asExpressionLeaf.value = valuePtr;
    astNode->value.asExpressionLeaf.subjectVariable = CATTO_NULL;
    astNode->value.asExpressionLeaf.firstArgument = CATTO_NULL;
    astNode->value.asExpressionLeaf.index = CATTO_NULL;

    return astNode;
}

CATTO_FN_PREFIX catto_AstNode* catto_parseExpressionLeaf(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr || !(
        (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_STRING ||
        (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_NUMBER ||
        (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_IDENTIFIER
    )) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_eat(currentTokenPtr);

    if (!token) {
        return CATTO_NULL;
    }

    catto_TypedValue* value = CATTO_NULL;
    catto_Char* subjectVariable = CATTO_NULL;
    catto_AstNode* firstArgument = CATTO_NULL;
    catto_AstNode* index = CATTO_NULL;

    switch (token->type) {
        case CATTO_TOKEN_TYPE_NUMBER:
            value = CATTO_NEW(catto_TypedValue);

            value->type = CATTO_DATA_TYPE_NUMBER;
            value->value.asNumber = token->value.asNumber;

            break;

        case CATTO_TOKEN_TYPE_STRING:
            value = CATTO_NEW(catto_TypedValue);

            value->type = CATTO_DATA_TYPE_STRING;
            value->value.asString = catto_copyString(token->value.asString);

            break;

        case CATTO_TOKEN_TYPE_IDENTIFIER:
            subjectVariable = catto_copyString(token->value.asString);

            if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPENING_BRACKET)) {
                catto_AstNode* currentArgument = CATTO_NULL;

                while (CATTO_TRUE) {
                    if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_CLOSING_BRACKET)) {
                        break;
                    }

                    if (firstArgument && !catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_DELIMETER)) {
                        return CATTO_NULL;
                    }

                    if (!catto_parseExpression(currentTokenPtr, &currentArgument)) {
                        return CATTO_NULL;
                    }

                    if (!firstArgument) {
                        firstArgument = currentArgument;
                    }
                }
            }

            if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPENING_ACCESSOR_BRACKET)) {
                if (!catto_parseExpression(currentTokenPtr, &index)) {
                    return CATTO_NULL;
                }

                if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_CLOSING_ACCESSOR_BRACKET)) {
                    return CATTO_NULL;
                }
            }

            break;

        default:
            return CATTO_NULL;
    }

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_EXPRESSION_LEAF, currentAstNodePtr);

    astNode->value.asExpressionLeaf.value = value;
    astNode->value.asExpressionLeaf.subjectVariable = subjectVariable;
    astNode->value.asExpressionLeaf.firstArgument = firstArgument;
    astNode->value.asExpressionLeaf.index = index;
    astNode->value.asExpressionLeaf.appendFlag = CATTO_FALSE;

    catto_Token* tokenPtrAfter = *currentTokenPtr ? (*currentTokenPtr)->nextToken : CATTO_NULL;

    if (
        *currentTokenPtr && (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_OPERATOR && catto_stringsEqualCaseInsensitive((*currentTokenPtr)->value.asString, ";") &&
        (
            !tokenPtrAfter || (tokenPtrAfter && (
                tokenPtrAfter->type == CATTO_TOKEN_TYPE_NEXT_LINE ||
                tokenPtrAfter->type == CATTO_TOKEN_TYPE_COMMENT ||
                tokenPtrAfter->type == CATTO_TOKEN_TYPE_DELIMETER ||
                tokenPtrAfter->type == CATTO_TOKEN_TYPE_STATEMENT_DELIMETER
            ))
        )
    ) {
        astNode->value.asExpressionLeaf.appendFlag = CATTO_TRUE;

        catto_eat(currentTokenPtr);
    }

    return astNode;
}

CATTO_FN_PREFIX catto_Bool catto_matchesInOperatorPrecedenceLevel(catto_Token* token, catto_Count level) {
    const catto_Char** operatorsAtLevel = catto_operatorPrecedence[level];

    if (!operatorsAtLevel || !token || token->type != CATTO_TOKEN_TYPE_OPERATOR) {
        return CATTO_FALSE;
    }

    catto_Count i = 0;

    while (operatorsAtLevel[i]) {
        if (catto_stringsEqualCaseInsensitive(operatorsAtLevel[i], token->value.asString)) {
            return CATTO_TRUE;
        }

        i++;
    }

    return CATTO_FALSE;
}

CATTO_FN_PREFIX catto_AstNode* catto_parseUnaryExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Token* operatorValue = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPERATOR);

    if (!operatorValue) {
        return CATTO_NULL;
    }

    catto_Count i = 0;
    catto_Bool operatorIsUnary = CATTO_FALSE;

    while (catto_unaryOperators[i]) {
        if (catto_stringsEqualCaseInsensitive(catto_unaryOperators[i], operatorValue->value.asString)) {
            operatorIsUnary = CATTO_TRUE;
            break;
        }

        i++;
    }

    if (!operatorIsUnary) {
        return CATTO_NULL;
    }

    catto_AstNode* child = CATTO_NULL;

    if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPENING_BRACKET)) {
        if (!catto_parseExpression(currentTokenPtr, &child)) {
            return CATTO_NULL;
        }

        if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_CLOSING_BRACKET)) {
            return CATTO_NULL;
        }
    } else {
        if (!catto_parseExpressionLeaf(currentTokenPtr, &child)) {
            return CATTO_NULL;
        }
    }

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_UNARY_EXPRESSION, currentAstNodePtr);

    astNode->value.asUnaryExpression.child = child;
    astNode->value.asUnaryExpression.operatorValue = operatorValue->value.asString;

    return astNode;
}

CATTO_FN_PREFIX catto_AstNode* catto_parseBinaryExpression(catto_Count operatorPrecedenceLevel, catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Char** operatorValues = (catto_Char**)CATTO_MALLOC(sizeof(catto_Char*));
    catto_Count operatorCount = 0;

    operatorValues[0] = CATTO_NULL;

    catto_AstNode* firstChild = CATTO_NULL;
    catto_AstNode* currentChild = CATTO_NULL;

    while (CATTO_TRUE) {
        if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPENING_BRACKET)) {
            if (!catto_parseExpression(currentTokenPtr, &currentChild)) {
                CATTO_FREE(operatorValues);

                return CATTO_NULL;
            }

            if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_CLOSING_BRACKET)) {
                CATTO_FREE(operatorValues);

                return CATTO_NULL;
            }
        } else if (*currentTokenPtr && (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_OPERATOR) {
            if (!catto_parseUnaryExpression(currentTokenPtr, &currentChild)) {
                CATTO_FREE(operatorValues);

                return CATTO_NULL;
            }
        } else if (catto_operatorPrecedence[operatorPrecedenceLevel + 1]) {
            if (!catto_parseBinaryExpression(operatorPrecedenceLevel + 1, currentTokenPtr, &currentChild)) {
                CATTO_FREE(operatorValues);

                return CATTO_NULL;
            }
        } else {
            if (!catto_parseExpressionLeaf(currentTokenPtr, &currentChild)) {
                CATTO_FREE(operatorValues);

                return CATTO_NULL;
            }
        }

        if (!firstChild) {
            firstChild = currentChild;
        }

        catto_Token* operatorValue = *currentTokenPtr;

        if (!operatorValue || operatorValue->type != CATTO_TOKEN_TYPE_OPERATOR) {
            break;
        }

        catto_Count lowerOperatorPrecedenceLevel = operatorPrecedenceLevel;
        catto_Bool reachedLowerPrecedenceLevelOperator = CATTO_FALSE;

        while (lowerOperatorPrecedenceLevel > 0) {
            if (catto_matchesInOperatorPrecedenceLevel(operatorValue, lowerOperatorPrecedenceLevel - 1)) {
                reachedLowerPrecedenceLevelOperator = CATTO_TRUE;
                break;
            }

            lowerOperatorPrecedenceLevel--;
        }

        if (reachedLowerPrecedenceLevelOperator) {
            break;
        }

        catto_eat(currentTokenPtr);

        operatorValues = (catto_Char**)CATTO_REALLOC(operatorValues, sizeof(catto_Char*) * (operatorCount + 2));
        operatorValues[operatorCount++] = operatorValue->value.asString;
        operatorValues[operatorCount] = CATTO_NULL;
    }

    if (operatorCount == 0) {
        if (*currentAstNodePtr) {
            (*currentAstNodePtr)->nextAstNode = firstChild;
        }

        CATTO_FREE(operatorValues);

        *currentAstNodePtr = firstChild;

        return firstChild;
    }

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_BINARY_EXPRESSION, currentAstNodePtr);

    astNode->value.asBinaryExpression.firstChild = firstChild;
    astNode->value.asBinaryExpression.operatorValues = operatorValues;

    return astNode;
}

CATTO_FN_PREFIX catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    return catto_parseBinaryExpression(0, currentTokenPtr, currentAstNodePtr);
}

CATTO_FN_PREFIX catto_AstNode* catto_parseStatement(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Token* lineNumberToken = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_LINE_NUMBER);
    catto_Token* commandToken = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_COMMAND);
    catto_AstNode* lastAstNode = *currentAstNodePtr;
    catto_Bool noop = CATTO_FALSE;

    if (commandToken) {
        catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_COMMAND_STATEMENT, currentAstNodePtr);

        astNode->value.asStatement.lineNumber = lineNumberToken ? lineNumberToken->value.asLineNumber : 0;
        astNode->value.asStatement.attributes.asCommandHandler = commandToken->value.asCommandHandler;
        astNode->value.asStatement.previousAstNode = lastAstNode;

        const catto_Char* commandName = commandToken->value.asCommandHandler->name;

        catto_AstNode* firstArgument = CATTO_NULL;
        catto_AstNode* currentArgument = CATTO_NULL;

        if (catto_stringsEqualCaseInsensitive(commandName, "def")) {
            firstArgument = catto_parseExpressionLeaf(currentTokenPtr, &currentArgument);
        }

        if (catto_stringsEqualCaseInsensitive(commandName, "else")) {
            firstArgument = catto_createExpressionLeaf(catto_asTypedNumber(0), &currentArgument);

            if (!catto_eatIfKeyword(currentTokenPtr, "if")) {
                goto finishAstNode;
            }

            catto_parseExpression(currentTokenPtr, &currentArgument);
        }

        if (catto_stringsEqualCaseInsensitive(commandName, "end")) {
            firstArgument = catto_createExpressionLeaf(catto_asTypedNumber(0), &currentArgument);
        }

        if (catto_stringsEqualCaseInsensitive(commandName, "for")) {
            firstArgument = catto_parseExpressionLeaf(currentTokenPtr, &currentArgument);

            if (!catto_eatIfKeyword(currentTokenPtr, "=")) {
                goto finishAstNode;
            }

            catto_parseExpression(currentTokenPtr, &currentArgument);

            if (!catto_eatIfKeyword(currentTokenPtr, "to")) {
                goto finishAstNode;
            }

            catto_parseExpression(currentTokenPtr, &currentArgument);

            if (catto_eatIfKeyword(currentTokenPtr, "step")) {
                catto_parseExpression(currentTokenPtr, &currentArgument);
            }
        }

        if (catto_stringsEqualCaseInsensitive(commandName, "while") || catto_stringsEqualCaseInsensitive(commandName, "until")) {
            firstArgument = catto_createExpressionLeaf(catto_asTypedNumber(0), &currentArgument);
        }

        finishAstNode:

        while (catto_parseExpression(currentTokenPtr, &currentArgument)) {
            if (!firstArgument) {
                firstArgument = currentArgument;
            }

            if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_DELIMETER)) {
                break;
            }
        }

        astNode->value.asStatement.firstArgument = firstArgument;

        return astNode;
    }

    catto_Token* identifierToken = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_IDENTIFIER);

    if (identifierToken) {
        catto_Char* subject = catto_copyString(identifierToken->value.asString);
        catto_AstNode* index = CATTO_NULL;
        catto_Bool parsedAccessor = CATTO_FALSE;

        if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPENING_ACCESSOR_BRACKET)) {
            catto_parseExpression(currentTokenPtr, &index);

            if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_CLOSING_ACCESSOR_BRACKET)) {
                CATTO_FREE(subject);
                goto syntaxError;
            }

            parsedAccessor = CATTO_TRUE;
        }

        catto_Token* assignmentOperatorToken = catto_eatIfKeyword(currentTokenPtr, "=");

        if (assignmentOperatorToken) {
            catto_AstNode* value = CATTO_NULL;

            if (!catto_parseExpression(currentTokenPtr, &value)) {
                CATTO_FREE(subject);
                goto syntaxError;
            }

            catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_ASSIGNMENT_STATEMENT, currentAstNodePtr);

            astNode->value.asStatement.lineNumber = lineNumberToken ? lineNumberToken->value.asLineNumber : 0;
            astNode->value.asStatement.firstArgument = value;
            astNode->value.asStatement.attributes.asAssignee.subjectVariable = subject;
            astNode->value.asStatement.attributes.asAssignee.index = index;
            astNode->value.asStatement.previousAstNode = lastAstNode;

            return astNode;
        } else {
            catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_PROCEDURE_STATEMENT, currentAstNodePtr);
            catto_AstNode* firstArgument = CATTO_NULL;
            catto_AstNode* currentArgument = CATTO_NULL;

            while (catto_parseExpression(currentTokenPtr, &currentArgument)) {
                if (!firstArgument) {
                    firstArgument = currentArgument;
                }

                if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_DELIMETER)) {
                    break;
                }
            }

            astNode->value.asStatement.lineNumber = lineNumberToken ? lineNumberToken->value.asLineNumber : 0;
            astNode->value.asStatement.firstArgument = firstArgument;
            astNode->value.asStatement.attributes.asProcedure.name = subject;
            astNode->value.asStatement.previousAstNode = lastAstNode;

            return astNode;
        }
    }

    noop = CATTO_TRUE;

    syntaxError: ;

    catto_AstNode* astNode = catto_addAstNode(noop ? CATTO_AST_NODE_TYPE_NOOP : CATTO_AST_NODE_TYPE_SYNTAX_ERROR, currentAstNodePtr);

    astNode->value.asStatement.lineNumber = lineNumberToken ? lineNumberToken->value.asLineNumber : 0;
    astNode->value.asStatement.previousAstNode = lastAstNode;

    catto_eat(currentTokenPtr);

    return astNode;
}

CATTO_FN_PREFIX catto_AstNode* catto_parse(catto_Token* firstToken) {
    catto_Token** currentTokenPtr = &firstToken;
    catto_AstNode* firstAstNode = CATTO_NULL;
    catto_AstNode* currentAstNode = CATTO_NULL;

    while (*currentTokenPtr) {
        if (catto_parseStatement(currentTokenPtr, &currentAstNode)) {
            while (
                catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_STATEMENT_DELIMETER) ||
                catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_NEXT_LINE)
            ) {}
        } else {
            catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_SYNTAX_ERROR, &currentAstNode);

            astNode->value.asStatement.lineNumber = 0;

            break;
        }

        if (currentAstNode && !firstAstNode) {
            firstAstNode = currentAstNode;
        }
    }

    return firstAstNode;
}

CATTO_FN_PREFIX catto_Bool catto_isCommand(catto_AstNode* astNode, const catto_Char* command) {
    if (astNode->type != CATTO_AST_NODE_TYPE_COMMAND_STATEMENT) {
        return CATTO_FALSE;
    }

    catto_CommandHandler* commandHandler = astNode->value.asStatement.attributes.asCommandHandler;

    return commandHandler && catto_stringsEqualCaseInsensitive(commandHandler->name, command);
}

CATTO_FN_PREFIX catto_TypedValue* catto_getMarkConditionSwitch(catto_AstNode* astNode) {
    if (astNode->type != CATTO_AST_NODE_TYPE_COMMAND_STATEMENT) {
        return CATTO_FALSE;
    }

    catto_AstNode* conditionSwitch = astNode->value.asStatement.firstArgument;

    if (conditionSwitch && conditionSwitch->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        catto_TypedValue* value = conditionSwitch->value.asExpressionLeaf.value;

        if (value && value->type == CATTO_DATA_TYPE_NUMBER) {
            return value;
        }
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_Bool catto_markConditionSwitchIsEnabled(catto_AstNode* astNode) {
    catto_TypedValue* value = catto_getMarkConditionSwitch(astNode);

    return value ? !!value->value.asNumber : CATTO_FALSE;
}

CATTO_FN_PREFIX catto_Bool catto_setMarkConditionSwitch(catto_AstNode* astNode, catto_Bool enabled) {
    catto_TypedValue* value = catto_getMarkConditionSwitch(astNode);

    if (!value) {
        return CATTO_FALSE;
    }

    value->value.asNumber = (catto_Float)enabled;

    return CATTO_TRUE;
}

CATTO_FN_PREFIX catto_Bool catto_isOpeningMark(catto_AstNode* astNode, catto_MarkSearchMode searchMode) {
    return (
        (searchMode != CATTO_MARK_SEARCH_LOOP_ONLY && catto_isCommand(astNode, "if")) ||
        catto_isCommand(astNode, "for") ||
        catto_isCommand(astNode, "repeat") ||
        (catto_isCommand(astNode, "while") && !catto_markConditionSwitchIsEnabled(astNode)) ||
        (catto_isCommand(astNode, "until") && !catto_markConditionSwitchIsEnabled(astNode))
    );
}

CATTO_FN_PREFIX catto_Bool catto_isClosingMark(catto_AstNode* astNode, catto_MarkSearchMode searchMode) {
    return (
        (searchMode != CATTO_MARK_SEARCH_LOOP_ONLY && catto_isCommand(astNode, "end")) ||
        catto_isCommand(astNode, "next") ||
        catto_isCommand(astNode, "loop") ||
        (catto_isCommand(astNode, "while") && catto_markConditionSwitchIsEnabled(astNode)) ||
        (catto_isCommand(astNode, "until") && catto_markConditionSwitchIsEnabled(astNode))
    );
}

CATTO_FN_PREFIX catto_AstNode* catto_findOpeningMark(catto_AstNode* astNode, const catto_Char* mark, catto_MarkSearchMode searchMode) {
    catto_Int depth = 0;

    if (astNode) {
        astNode = astNode->value.asStatement.previousAstNode;
    }

    while (astNode) {
        if (depth < 0) {
            break;
        }

        if (depth == 0 && (
            (mark && catto_isCommand(astNode, mark)) ||
            (!mark && catto_isOpeningMark(astNode, searchMode))
        )) {
            return astNode;
        }

        if (catto_isClosingMark(astNode, searchMode)) {
            depth++;
        }

        if (catto_isOpeningMark(astNode, searchMode)) {
            depth--;
        }

        astNode = astNode->value.asStatement.previousAstNode;
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX catto_AstNode* catto_findClosingMark(catto_AstNode* astNode, const catto_Char* mark, catto_MarkSearchMode searchMode) {
    catto_Int depth = 0;

    if (astNode) {
        astNode = astNode->nextAstNode;
    }

    while (astNode) {
        if (depth < 0) {
            break;
        }

        if (depth == 0 && (
            (mark && catto_isCommand(astNode, mark)) ||
            (!mark && catto_isClosingMark(astNode, searchMode))
        )) {
            return astNode;
        }

        if (catto_isOpeningMark(astNode, searchMode)) {
            depth++;
        }

        if (catto_isClosingMark(astNode, searchMode)) {
            depth--;
        }

        astNode = astNode->nextAstNode;
    }

    return CATTO_NULL;
}

CATTO_FN_PREFIX void catto_freeAstNodes(catto_AstNode* firstAstNode) {
    catto_AstNode* currentAstNode = firstAstNode;

    while (currentAstNode) {
        catto_AstNode* lastAstNode = currentAstNode;

        switch (currentAstNode->type) {
            case CATTO_AST_NODE_TYPE_COMMAND_STATEMENT:
                catto_freeAstNodes(currentAstNode->value.asStatement.firstArgument);
                break;

            case CATTO_AST_NODE_TYPE_PROCEDURE_STATEMENT:
                catto_freeAstNodes(currentAstNode->value.asStatement.firstArgument);

                CATTO_FREE(currentAstNode->value.asStatement.attributes.asProcedure.name);

                break;

            case CATTO_AST_NODE_TYPE_ASSIGNMENT_STATEMENT:
                catto_freeAstNodes(currentAstNode->value.asStatement.firstArgument);
                catto_freeAstNodes(currentAstNode->value.asStatement.attributes.asAssignee.index);

                CATTO_FREE(currentAstNode->value.asStatement.attributes.asAssignee.subjectVariable);

                break;

            case CATTO_AST_NODE_TYPE_EXPRESSION_LEAF:
                catto_freeTypedValue(currentAstNode->value.asExpressionLeaf.value);

                catto_freeAstNodes(currentAstNode->value.asExpressionLeaf.firstArgument);
                catto_freeAstNodes(currentAstNode->value.asExpressionLeaf.index);

                CATTO_FREE(currentAstNode->value.asExpressionLeaf.subjectVariable);

                break;

            case CATTO_AST_NODE_TYPE_UNARY_EXPRESSION:
                catto_freeAstNodes(currentAstNode->value.asUnaryExpression.child);

                break;

            case CATTO_AST_NODE_TYPE_BINARY_EXPRESSION:
                catto_freeAstNodes(currentAstNode->value.asBinaryExpression.firstChild);

                CATTO_FREE(currentAstNode->value.asBinaryExpression.operatorValues);

                break;

            default:
                break;
        }

        currentAstNode = currentAstNode->nextAstNode;

        CATTO_FREE(lastAstNode);
    }
}

CATTO_FN_PREFIX void catto_debugAstNodes(catto_AstNode* firstAstNode) {
    catto_AstNode* currentAstNode = firstAstNode;
    catto_Bool hadFirst = CATTO_FALSE;

    while (currentAstNode) {
        if (hadFirst) {
            CATTO_LOG_CHAR(' ');
        } else {
            hadFirst = CATTO_TRUE;
        }

        switch (currentAstNode->type) {
            case CATTO_AST_NODE_TYPE_SYNTAX_ERROR:
                CATTO_LOG("[error]");
                break;

            case CATTO_AST_NODE_TYPE_NOOP:
                CATTO_LOG("[noop]");
                break;

            case CATTO_AST_NODE_TYPE_COMMAND_STATEMENT:
                if (currentAstNode->value.asStatement.attributes.asCommandHandler) {
                    CATTO_LOG(currentAstNode->value.asStatement.attributes.asCommandHandler->name);
                } else {
                    CATTO_LOG("[unkn]");
                }

                CATTO_LOG_CHAR('(');

                catto_debugAstNodes(currentAstNode->value.asStatement.firstArgument);

                CATTO_LOG_CHAR(')');

                break;

            case CATTO_AST_NODE_TYPE_PROCEDURE_STATEMENT:
                CATTO_LOG(currentAstNode->value.asStatement.attributes.asProcedure.name);

                CATTO_LOG_CHAR('(');

                catto_debugAstNodes(currentAstNode->value.asStatement.firstArgument);

                CATTO_LOG_CHAR(')');

                break;

            case CATTO_AST_NODE_TYPE_EXPRESSION_LEAF:
                if (currentAstNode->value.asExpressionLeaf.subjectVariable) {
                    CATTO_LOG(currentAstNode->value.asExpressionLeaf.subjectVariable);
                } else if (currentAstNode->value.asExpressionLeaf.value) {
                    CATTO_LOG_CHAR(currentAstNode->value.asExpressionLeaf.value->type);
                } else {
                    CATTO_LOG_CHAR('e');
                }

                if (currentAstNode->value.asExpressionLeaf.firstArgument) {
                    CATTO_LOG_CHAR('(');

                    catto_debugAstNodes(currentAstNode->value.asExpressionLeaf.firstArgument);

                    CATTO_LOG_CHAR(')');
                }

                if (currentAstNode->value.asExpressionLeaf.index) {
                    CATTO_LOG_CHAR('[');

                    catto_debugAstNodes(currentAstNode->value.asExpressionLeaf.index);

                    CATTO_LOG_CHAR(']');
                }

                break;

            case CATTO_AST_NODE_TYPE_UNARY_EXPRESSION:
                CATTO_LOG(currentAstNode->value.asUnaryExpression.operatorValue);
                CATTO_LOG_CHAR('(');

                catto_debugAstNodes(currentAstNode->value.asUnaryExpression.child);

                CATTO_LOG_CHAR(')');

                break;

            case CATTO_AST_NODE_TYPE_BINARY_EXPRESSION:
                CATTO_LOG(currentAstNode->value.asBinaryExpression.operatorValues[0]);
                CATTO_LOG_CHAR('(');

                catto_debugAstNodes(currentAstNode->value.asBinaryExpression.firstChild);

                CATTO_LOG_CHAR(')');

                break;

            default:
                CATTO_LOG_CHAR(currentAstNode->type);
                break;
        }

        currentAstNode = currentAstNode->nextAstNode;
    }
}

// src/stdlib/controlflow.h

CATTO_FN_PREFIX void catto_command_goto(catto_Context* context) {
    catto_Int lineNumber = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (lineNumber < 0) {
        lineNumber = 0;
    }

    catto_goto(context, lineNumber);
}

CATTO_FN_PREFIX void catto_command_gosub(catto_Context* context) {
    catto_Int lineNumber = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (lineNumber < 0) {
        lineNumber = 0;
    }

    catto_pushOntoStatementStack(context, context->nextParsedStatement);

    catto_goto(context, lineNumber);
}

CATTO_FN_PREFIX void catto_command_def(catto_Context* context) {
    catto_AstNode* name = catto_getNextArg(context);
    catto_Char* subject;

    if (
        !name ||
        name->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF ||
        !(subject = name->value.asExpressionLeaf.subjectVariable)
    ) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end", CATTO_MARK_SEARCH_ALL);

    if (!endStatement) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    catto_setMarkConditionSwitch(endStatement, CATTO_TRUE);

    catto_Procedure* procedure = catto_getProcedure(context, subject);

    if (!procedure) {
        procedure = catto_createProcedure(context, subject);
    }

    if (procedure->parameterCount > 0) {
        procedure->parameterCount = 0;
        procedure->parameterNames = (catto_Char**)CATTO_REALLOC(procedure->parameterNames, 0);
    }

    while (catto_hasNextArg(context)) {
        catto_AstNode* parameterName = catto_getNextArg(context);
        catto_Char* parameter;

        if (
            !parameterName ||
            parameterName->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF ||
            !(parameter = parameterName->value.asExpressionLeaf.subjectVariable)
        ) {
            context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
            return;
        }

        procedure->parameterCount++;
        procedure->parameterNames = (catto_Char**)CATTO_REALLOC(procedure->parameterNames, procedure->parameterCount * sizeof(catto_Char*));
        procedure->parameterNames[procedure->parameterCount - 1] = parameter;
    }

    procedure->astNode = context->nextParsedStatement;

    context->nextParsedStatement = endStatement->nextAstNode;
}

CATTO_FN_PREFIX void catto_command_return(catto_Context* context) {
    if (context->statementStackCount == 0) {
        context->errorState = CATTO_ERROR_STATE_NO_RETURN;
        return;
    }

    context->nextParsedStatement = catto_popFromStatementStack(context);
}

CATTO_FN_PREFIX void catto_command_if(catto_Context* context) {
    catto_Bool isTrue = catto_asBool(catto_evalNextArg(context));

    catto_AstNode* elseStatement = catto_findClosingMark(context->currentParsedStatement, "else", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end", CATTO_MARK_SEARCH_ALL);

    if (elseStatement) {
        catto_setMarkConditionSwitch(elseStatement, isTrue);
    }

    if (!endStatement) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    if (!isTrue) {
        if (elseStatement) {
            context->nextParsedStatement = elseStatement;
            return;
        }

        context->nextParsedStatement = endStatement;
        return;
    }
}

CATTO_FN_PREFIX void catto_command_else(catto_Context* context) {
    catto_Bool shouldSkip = catto_asBool(catto_evalNextArg(context));
    catto_Bool isElseIf = catto_hasNextArg(context);

    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end", CATTO_MARK_SEARCH_ALL);

    if (!endStatement) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    if (shouldSkip) {
        context->nextParsedStatement = endStatement;
        return;
    }

    if (!isElseIf) {
        return;
    }

    catto_Bool isTrue = catto_asBool(catto_evalNextArg(context));

    catto_AstNode* elseStatement = catto_findClosingMark(context->currentParsedStatement, "else", CATTO_MARK_SEARCH_ALL);

    if (elseStatement) {
        catto_AstNode* conditionSwitch = elseStatement->value.asStatement.firstArgument;

        if (conditionSwitch && conditionSwitch->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
            catto_TypedValue* value = conditionSwitch->value.asExpressionLeaf.value;

            if (value && value->type == CATTO_DATA_TYPE_NUMBER) {
                value->value.asNumber = (catto_Float)isTrue;
            }
        }
    }

    if (!isTrue) {
        if (elseStatement) {
            context->nextParsedStatement = elseStatement;
            return;
        }

        context->nextParsedStatement = endStatement;
        return;
    }
}

CATTO_FN_PREFIX void catto_command_end(catto_Context* context) {
    catto_Bool isProcedureReturn = catto_asBool(catto_evalNextArg(context));

    if (isProcedureReturn) {
        if (context->statementStackCount == 0) {
            context->errorState = CATTO_ERROR_STATE_NO_RETURN;
            return;
        }

        context->nextParsedStatement = catto_popFromStatementStack(context);
    }
}

CATTO_FN_PREFIX void catto_command_for(catto_Context* context) {
    if (!catto_findClosingMark(context->currentParsedStatement, "next", CATTO_MARK_SEARCH_ALL)) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    catto_AstNode* identifier = catto_getNextArg(context);
    catto_TypedValue start = catto_evalNextArg(context);

    if (identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_assignValue(context, identifier, start);
}

CATTO_FN_PREFIX void catto_command_next(catto_Context* context) {
    catto_AstNode* forStatement = catto_findOpeningMark(context->currentParsedStatement, "for", CATTO_MARK_SEARCH_ALL);

    if (!forStatement) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_CLOSING_MARK;
        return;
    }

    context->nextParsedArgument = forStatement->value.asStatement.firstArgument;

    catto_AstNode* identifier = catto_getNextArg(context);
    catto_TypedValue start = catto_evalNextArg(context);
    catto_TypedValue stop = catto_evalNextArg(context);

    catto_TypedValue step = catto_hasNextArg(context) ? catto_evalNextArg(context) : (
        catto_asNumber(stop) < catto_asNumber(start) ?
        catto_asTypedNumber(-1) :
        catto_asTypedNumber(1)
    );

    if (!identifier || identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_Float currentValue = catto_asNumber(catto_evalExpression(context, identifier));

    if (
        (catto_asNumber(step) >= 0 && currentValue >= catto_asNumber(stop)) ||
        (catto_asNumber(step) < 0 && currentValue <= catto_asNumber(stop))
    ) {
        return;
    }

    catto_assignValue(context, identifier, catto_asTypedNumber(currentValue + catto_asNumber(step)));

    context->nextParsedStatement = forStatement->nextAstNode;
}

CATTO_FN_PREFIX void catto_command_repeat(catto_Context* context) {
    catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_ALL);

    if (whileStatement) {
        catto_setMarkConditionSwitch(whileStatement, CATTO_TRUE);
        return;
    }
    
    if (untilStatement) {
        catto_setMarkConditionSwitch(untilStatement, CATTO_TRUE);
        return;
    }

    if (loopStatement) {
        return;
    }

    context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
}

CATTO_FN_PREFIX void catto_command_whileOrUntil(catto_Context* context, catto_Bool isUntil) {
    catto_Bool isClosing = catto_asBool(catto_evalNextArg(context));
    catto_Bool isTrue = catto_asBool(catto_evalNextArg(context));

    if (isUntil) {
        isTrue = !isTrue;
    }

    if (isClosing) {
        if (isTrue) {
            catto_AstNode* repeatStatement = catto_findOpeningMark(context->currentParsedStatement, "repeat", CATTO_MARK_SEARCH_ALL);

            if (repeatStatement) {
                context->nextParsedStatement = repeatStatement;
            } else {
                context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
            }
        }

        return;
    }

    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_ALL);

    if (!loopStatement) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    if (!isTrue) {
        context->nextParsedStatement = loopStatement->nextAstNode;
        return;
    }
}

CATTO_FN_PREFIX void catto_command_while(catto_Context* context) {
    catto_command_whileOrUntil(context, CATTO_FALSE);
}

CATTO_FN_PREFIX void catto_command_until(catto_Context* context) {
    catto_command_whileOrUntil(context, CATTO_TRUE);
}

CATTO_FN_PREFIX void catto_command_loop(catto_Context* context) {
    catto_AstNode* repeatStatement = catto_findOpeningMark(context->currentParsedStatement, "repeat", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* whileStatement = catto_findOpeningMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* untilStatement = catto_findOpeningMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_ALL);

    if (repeatStatement) {
        context->nextParsedStatement = repeatStatement;
        return;
    }

    if (whileStatement) {
        context->nextParsedStatement = whileStatement;
        return;
    }

    if (untilStatement) {
        context->nextParsedStatement = untilStatement;
        return;
    }

    context->errorState = CATTO_ERROR_STATE_MISMATCHED_CLOSING_MARK;
}

CATTO_FN_PREFIX void catto_command_break(catto_Context* context) {
    catto_AstNode* openingStatement = catto_findOpeningMark(context->currentParsedStatement, CATTO_NULL, CATTO_MARK_SEARCH_LOOP_ONLY);

    if (!openingStatement) {
        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    if (catto_isCommand(openingStatement, "repeat")) {
        catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

        if (whileStatement) {
            context->nextParsedStatement = whileStatement->nextAstNode;
            return;
        }

        if (untilStatement) {
            context->nextParsedStatement = untilStatement->nextAstNode;
            return;
        }

        if (loopStatement) {
            context->nextParsedStatement = loopStatement->nextAstNode;
            return;
        }

        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    catto_AstNode* nextStatement = catto_findClosingMark(context->currentParsedStatement, "next", CATTO_MARK_SEARCH_LOOP_ONLY);
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

    if (nextStatement) {
        context->nextParsedStatement = nextStatement->nextAstNode;
        return;
    }

    if (loopStatement) {
        context->nextParsedStatement = loopStatement->nextAstNode;
        return;
    }

    context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
}

CATTO_FN_PREFIX void catto_command_continue(catto_Context* context) {
    catto_AstNode* openingStatement = catto_findOpeningMark(context->currentParsedStatement, CATTO_NULL, CATTO_MARK_SEARCH_LOOP_ONLY);

    if (!openingStatement) {
        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    if (catto_isCommand(openingStatement, "repeat")) {
        catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

        if (whileStatement) {
            context->nextParsedStatement = whileStatement;
            return;
        }

        if (untilStatement) {
            context->nextParsedStatement = untilStatement;
            return;
        }

        if (loopStatement) {
            context->nextParsedStatement = loopStatement;
            return;
        }

        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    catto_AstNode* nextStatement = catto_findClosingMark(context->currentParsedStatement, "next", CATTO_MARK_SEARCH_LOOP_ONLY);
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

    if (nextStatement) {
        context->nextParsedStatement = nextStatement;
        return;
    }

    if (loopStatement) {
        context->nextParsedStatement = loopStatement;
        return;
    }

    context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
}

CATTO_FN_PREFIX void catto_command_stop(catto_Context* context) {
    context->nextParsedStatement = CATTO_NULL;
}

// src/stdlib/io.h

#ifndef CATTO_CUSTOM_PRINT_COMMAND
CATTO_FN_PREFIX void catto_command_print(catto_Context* context) {
    catto_Bool appendFlag = CATTO_FALSE;

    while (catto_hasNextArg(context)) {
        catto_AstNode* arg = catto_getNextArg(context);
        catto_Char* string = catto_asString(catto_evalExpression(context, arg));

        if (!catto_hasNextArg(context) && catto_hasAppendFlag(arg)) {
            appendFlag = CATTO_TRUE;
        }

        CATTO_LOG(string);
        CATTO_FREE(string);

        if (catto_hasNextArg(context)) {
            CATTO_LOG(" ");
        }
    }

    if (!appendFlag) {
        CATTO_LOG("\n");
    }
}
#endif

CATTO_FN_PREFIX void catto_command_scrawl(catto_Context* context) {
    context->scrawlMode = CATTO_TRUE;
}

CATTO_FN_PREFIX void catto_command_noscrawl(catto_Context* context) {
    context->scrawlMode = CATTO_FALSE;
}

// src/stdlib/lists.h

CATTO_FN_PREFIX void catto_command_dim(catto_Context* context) {
    catto_AstNode* identifier = catto_getNextArg(context);

    if (identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_TypedValue listValue = {
        .type = CATTO_DATA_TYPE_LIST,
        .value = {.asList = catto_newList()}
    };

    catto_setVariable(context, identifier->value.asExpressionLeaf.subjectVariable, listValue);
}

CATTO_FN_PREFIX void catto_command_push(catto_Context* context) {
    catto_TypedValue value = catto_evalNextArg(context);
    catto_TypedValue listValue = catto_evalNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_INVALID_LIST_VALUE;
        return;
    }

    catto_pushOntoList(listValue.value.asList, value);
}

CATTO_FN_PREFIX void catto_command_pop(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_AstNode* reassignedIdentifier = catto_getNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_TypedValue poppedValue = catto_popFromList(context, listValue.value.asList);

    if (reassignedIdentifier) {
        catto_assignValue(context, reassignedIdentifier, poppedValue);
    }
}

CATTO_FN_PREFIX void catto_command_insert(catto_Context* context) {
    catto_TypedValue value = catto_evalNextArg(context);
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_Int index = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_INVALID_LIST_VALUE;
        return;
    }

    while (index < 0) {
        index += listValue.value.asList->length;
    }

    catto_insertIntoList(listValue.value.asList, value, index);
}

CATTO_FN_PREFIX void catto_command_remove(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_Int index = (catto_Int)catto_asNumber(catto_evalNextArg(context));
    catto_AstNode* reassignedIdentifier = catto_getNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    while (index < 0) {
        index += listValue.value.asList->length;
    }

    catto_TypedValue removedValue = catto_removeFromList(context, listValue.value.asList, index);

    if (reassignedIdentifier && reassignedIdentifier->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        catto_assignValue(context, reassignedIdentifier, removedValue);
    }
}

// src/stdlib/functions.h

#define CATTO_TRIG_MODE_COMMAND(name, mode) CATTO_FN_PREFIX void name(catto_Context* context) { \
        context->trigMode = mode; \
    }

#define CATTO_UNARY_NUMERIC_FUNCTION(name, callName) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_DataType returnType) { \
        catto_Float value = catto_asNumber(catto_evalNextArg(context)); \
        \
        return catto_asTypedNumber(callName(value)); \
    }

#define CATTO_TRIG_FUNCTION(name, callName) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_DataType returnType) { \
        catto_Float value = catto_asNumber(catto_evalNextArg(context)); \
        catto_Float convertedValue = catto_toRadians(value, context->trigMode); \
        \
        return catto_asTypedNumber(callName(convertedValue)); \
    }

#define CATTO_TRIG_ARC_FUNCTION(name, callName) CATTO_FN_PREFIX catto_TypedValue name(catto_Context* context, catto_DataType returnType) { \
        catto_Float value = catto_asNumber(catto_evalNextArg(context)); \
        \
        return catto_asTypedNumber(catto_fromRadians(callName(value), context->trigMode)); \
    }

CATTO_TRIG_MODE_COMMAND(catto_command_deg, CATTO_TRIG_MODE_DEGREES);
CATTO_TRIG_MODE_COMMAND(catto_command_rad, CATTO_TRIG_MODE_RADIANS);
CATTO_TRIG_MODE_COMMAND(catto_command_gon, CATTO_TRIG_MODE_GRADIANS);
CATTO_TRIG_MODE_COMMAND(catto_command_turn, CATTO_TRIG_MODE_TURNS);

CATTO_TRIG_FUNCTION(catto_function_sin, catto_sin);
CATTO_TRIG_FUNCTION(catto_function_cos, catto_cos);
CATTO_TRIG_FUNCTION(catto_function_tan, catto_tan);

CATTO_TRIG_ARC_FUNCTION(catto_function_asin, catto_asin);
CATTO_TRIG_ARC_FUNCTION(catto_function_acos, catto_acos);
CATTO_TRIG_ARC_FUNCTION(catto_function_atan, catto_atan);

CATTO_UNARY_NUMERIC_FUNCTION(catto_function_log, catto_log);
CATTO_UNARY_NUMERIC_FUNCTION(catto_function_ln, catto_ln);
CATTO_UNARY_NUMERIC_FUNCTION(catto_function_sqrt, catto_sqrt);

CATTO_FN_PREFIX catto_TypedValue catto_function_round(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));
    catto_Int roundedValue = (catto_Int)(value < 0 ? value - 0.5 : value + 0.5);

    return catto_asTypedNumber((catto_Float)roundedValue);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_floor(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));
    catto_Int flooredValue = (catto_Int)(value < 0 ? value - 1 : value);

    return catto_asTypedNumber((catto_Float)flooredValue);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_ceil(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));
    catto_Int flooredValue = (catto_Int)(value < 0 ? value - 1 : value);

    return catto_asTypedNumber((catto_Float)(value == flooredValue ? flooredValue : flooredValue + 1));
}

CATTO_FN_PREFIX catto_TypedValue catto_function_abs(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));

    if (value < 0) {
        value *= -1;
    }

    return catto_asTypedNumber(value);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_min(catto_Context* context, catto_DataType returnType) {
    catto_Float a = catto_asNumber(catto_evalNextArg(context));
    catto_Float b = catto_asNumber(catto_evalNextArg(context));

    return catto_asTypedNumber(b < a ? b : a);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_max(catto_Context* context, catto_DataType returnType) {
    catto_Float a = catto_asNumber(catto_evalNextArg(context));
    catto_Float b = catto_asNumber(catto_evalNextArg(context));

    return catto_asTypedNumber(b > a ? b : a);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_asc(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));

    catto_TypedValue returnValue = catto_asTypedNumber(value[0]);

    CATTO_FREE(value);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_chr(catto_Context* context, catto_DataType returnType) {
    catto_Int codepoint = catto_asNumber(catto_evalNextArg(context));
    catto_Char* string = catto_copyString("");

    if (codepoint > 0) {
        string = catto_appendCharToString(string, codepoint);
    }

    catto_TypedValue returnValue = catto_asTypedString(string);

    CATTO_FREE(string);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_base(catto_Context* context, catto_Count base, catto_DataType returnType) {
    if (returnType == CATTO_DATA_TYPE_STRING) {
        catto_Float number = catto_asNumber(catto_evalNextArg(context));
        catto_Char* string = catto_numberToBaseString(number, base);
        catto_TypedValue returnValue = catto_asTypedString(string);

        CATTO_FREE(string);

        return returnValue;
    }

    catto_Char* string = catto_asString(catto_evalNextArg(context));

    catto_Count charactersEaten;
    catto_TypedValue returnValue = catto_asTypedNumber(catto_stringToBaseNumber(string, base, &charactersEaten));

    CATTO_FREE(string);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_bin(catto_Context* context, catto_DataType returnType) {
    return catto_function_base(context, 2, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_oct(catto_Context* context, catto_DataType returnType) {
    return catto_function_base(context, 8, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_hex(catto_Context* context, catto_DataType returnType) {
    return catto_function_base(context, 16, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_len(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue value = catto_evalNextArg(context);

    if (value.type == CATTO_DATA_TYPE_LIST) {
        return catto_asTypedNumber(value.value.asList->length);
    }

    return catto_asTypedNumber(catto_stringLength(catto_asString(value)));
}

CATTO_FN_PREFIX catto_TypedValue catto_function_last(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue value = catto_evalNextArg(context);

    if (value.type != CATTO_DATA_TYPE_LIST) {
        return catto_asTypedNumber(0);
    }

    catto_List* list = value.value.asList;

    if (list->length == 0) {
        return catto_asTypedNumber(0);
    }

    catto_TypedValue returnValue = catto_copyTypedValue(list->values[list->length - 1]);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_split(catto_Context* context, catto_DataType returnType) {
    catto_Char* string = catto_asString(catto_evalNextArg(context));
    catto_Char* delimeter = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString("");
    catto_Count delimeterLength = catto_stringLength(delimeter);
    catto_List* list = catto_newList();
    catto_Char* currentString = catto_copyString("");

    catto_Count i = 0;

    while (CATTO_TRUE) {
        if (!string[i]) {
            goto splitHere;
        }
        
        if (delimeterLength > 0 && catto_stringStartsWith(string + i, delimeter)) {
            goto splitHere;
        }

        currentString = catto_appendCharToString(currentString, string[i]);

        i++;

        if (delimeterLength > 0) {
            continue;
        }

        splitHere: ;

        catto_TypedValue typedString = catto_asTypedString(currentString);

        catto_pushOntoList(list, typedString);
        catto_addTypedValueToGc(context, typedString);

        CATTO_FREE(currentString);

        currentString = catto_copyString("");

        if (!string[i]) {
            break;
        }

        i += delimeterLength;

        continue;
    }

    CATTO_FREE(string);
    CATTO_FREE(delimeter);
    CATTO_FREE(currentString);

    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_LIST,
        .value = {.asList = list}
    };
}

CATTO_FN_PREFIX catto_TypedValue catto_function_join(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue value = catto_evalNextArg(context);

    if (value.type != CATTO_DATA_TYPE_LIST) {
        return catto_asTypedString("");
    }

    catto_List* list = value.value.asList;
    catto_Char* resultString = catto_copyString("");
    catto_Char* delimeter = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString("");
    catto_Count delimeterLength = catto_stringLength(delimeter);

    for (catto_Count i = 0; i < list->length; i++) {
        catto_Char* partString = catto_asString(list->values[i]);

        if (i > 0 && delimeterLength > 0) {
            resultString = catto_appendToString(resultString, delimeter);
        }

        resultString = catto_appendToString(resultString, partString);

        CATTO_FREE(partString);
    }

    catto_TypedValue returnValue = catto_asTypedString(resultString);

    CATTO_FREE(resultString);
    CATTO_FREE(delimeter);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_find(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue sequence = catto_evalNextArg(context);
    catto_TypedValue searchValue = catto_evalNextArg(context);

    if (sequence.type == CATTO_DATA_TYPE_LIST) {
        catto_List* list = sequence.value.asList;

        for (catto_Count i = 0; i < list->length; i++) {
            catto_TypedValue item = list->values[i];

            if (catto_asNumber(catto_binary_equal(context, item, searchValue))) {
                return catto_asTypedNumber(i);
            }
        }

        return catto_asTypedNumber(-1);
    }

    catto_Char* string = catto_asString(sequence);
    catto_Char* searchString = catto_asString(searchValue);
    catto_Int index = -1;

    for (catto_Count i = 0; i < catto_stringLength(string); i++) {
        if (catto_stringStartsWith(string + i, searchString)) {
            index = i;
            break;
        }
    }

    CATTO_FREE(string);
    CATTO_FREE(searchString);

    return catto_asTypedNumber(index);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_lower(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Char* currentChar = value;

    while (*currentChar != '\0') {
        if (*currentChar >= 'A' && *currentChar <= 'Z') {
            *currentChar += 'a' - 'A';
        }

        currentChar++;
    }

    catto_TypedValue returnValue = catto_asTypedString(value);

    CATTO_FREE(value);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_upper(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Char* currentChar = value;

    while (*currentChar != '\0') {
        if (*currentChar >= 'a' && *currentChar <= 'z') {
            *currentChar -= 'a' - 'A';
        }

        currentChar++;
    }

    catto_TypedValue returnValue = catto_asTypedString(value);

    CATTO_FREE(value);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_slicer(catto_Context* context, catto_SlicingMethod method, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Char* result = catto_copyString("");
    catto_Int currentLength = catto_stringLength(value);
    catto_Int startIndex = 0;
    catto_Int endIndex = currentLength;
    catto_Int length = 0;

    if (method == CATTO_SLICING_METHOD_LEFT) {
        endIndex = catto_asNumber(catto_evalNextArg(context));
    } else if (method == CATTO_SLICING_METHOD_RIGHT) {
        startIndex = currentLength - catto_asNumber(catto_evalNextArg(context));
    } else {
        startIndex = catto_asNumber(catto_evalNextArg(context));
        endIndex = startIndex + catto_asNumber(catto_evalNextArg(context));
    }

    if (endIndex > currentLength) {
        endIndex = currentLength;
    }

    for (catto_Int i = startIndex; i < endIndex; i++) {
        if (i < 0) {
            continue;
        }

        result = catto_appendCharToString(result, value[i]);
    }

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(result);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_left(catto_Context* context, catto_DataType returnType) {
    return catto_function_slicer(context, CATTO_SLICING_METHOD_LEFT, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_right(catto_Context* context, catto_DataType returnType) {
    return catto_function_slicer(context, CATTO_SLICING_METHOD_RIGHT, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_mid(catto_Context* context, catto_DataType returnType) {
    return catto_function_slicer(context, CATTO_SLICING_METHOD_MID, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_trimmer(catto_Context* context, catto_Bool trimLeft, catto_Bool trimRight, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Count currentLength = catto_stringLength(value);
    catto_Char* result = catto_copyString("");

    catto_Count startIndex = 0;
    catto_Count endIndex = currentLength;

    while (trimLeft && startIndex < currentLength) {
        if (value[startIndex] == ' ') {
            startIndex++;
        } else {
            break;
        }
    }

    while (trimRight && endIndex > 1) {
        if (value[endIndex - 1] == ' ') {
            endIndex--;
        } else {
            break;
        }
    }

    for (catto_Int i = startIndex; i < endIndex; i++) {
        if (i < 0) {
            continue;
        }

        result = catto_appendCharToString(result, value[i]);
    }

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(result);

    return returnValue;
}

CATTO_FN_PREFIX catto_TypedValue catto_function_trim(catto_Context* context, catto_DataType returnType) {
    return catto_function_trimmer(context, CATTO_TRUE, CATTO_TRUE, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_ltrim(catto_Context* context, catto_DataType returnType) {
    return catto_function_trimmer(context, CATTO_TRUE, CATTO_FALSE, returnType);
}

CATTO_FN_PREFIX catto_TypedValue catto_function_rtrim(catto_Context* context, catto_DataType returnType) {
    return catto_function_trimmer(context, CATTO_FALSE, CATTO_TRUE, returnType);
}

// src/stdlib/stdlib.h

CATTO_FN_PREFIX void catto_addContextStandardCommands(catto_Context* context) {
    // Control flow

    catto_addCommand(context, "goto", &catto_command_goto);
    catto_addCommand(context, "gosub", &catto_command_gosub);
    catto_addCommand(context, "def", &catto_command_def);
    catto_addCommand(context, "return", &catto_command_return);
    catto_addCommand(context, "if", &catto_command_if);
    catto_addCommand(context, "else", &catto_command_else);
    catto_addCommand(context, "end", &catto_command_end);
    catto_addCommand(context, "for", &catto_command_for);
    catto_addCommand(context, "next", &catto_command_next);
    catto_addCommand(context, "repeat", &catto_command_repeat);
    catto_addCommand(context, "while", &catto_command_while);
    catto_addCommand(context, "until", &catto_command_until);
    catto_addCommand(context, "loop", &catto_command_loop);
    catto_addCommand(context, "break", &catto_command_break);
    catto_addCommand(context, "continue", &catto_command_continue);
    catto_addCommand(context, "stop", &catto_command_stop);

    // I/O

    #ifndef CATTO_CUSTOM_PRINT_COMMAND
        catto_addCommand(context, "print", &catto_command_print);
    #endif

    catto_addCommand(context, "scrawl", &catto_command_scrawl);
    catto_addCommand(context, "noscrawl", &catto_command_noscrawl);

    // Lists

    catto_addCommand(context, "dim", &catto_command_dim);
    catto_addCommand(context, "push", &catto_command_push);
    catto_addCommand(context, "pop", &catto_command_pop);
    catto_addCommand(context, "insert", &catto_command_insert);
    catto_addCommand(context, "remove", &catto_command_remove);

    // Functions

    catto_addCommand(context, "deg", &catto_command_deg);
    catto_addCommand(context, "rad", &catto_command_rad);
    catto_addCommand(context, "gon", &catto_command_gon);
    catto_addCommand(context, "turn", &catto_command_turn);

    catto_addFunction(context, "sin", &catto_function_sin);
    catto_addFunction(context, "cos", &catto_function_cos);
    catto_addFunction(context, "tan", &catto_function_tan);
    catto_addFunction(context, "asin", &catto_function_asin);
    catto_addFunction(context, "acos", &catto_function_acos);
    catto_addFunction(context, "atan", &catto_function_atan);
    catto_addFunction(context, "log", &catto_function_log);
    catto_addFunction(context, "ln", &catto_function_ln);
    catto_addFunction(context, "sqrt", &catto_function_sqrt);
    catto_addFunction(context, "round", &catto_function_round);
    catto_addFunction(context, "floor", &catto_function_floor);
    catto_addFunction(context, "ceil", &catto_function_ceil);
    catto_addFunction(context, "abs", &catto_function_abs);
    catto_addFunction(context, "min", &catto_function_min);
    catto_addFunction(context, "max", &catto_function_max);
    catto_addFunction(context, "asc", &catto_function_asc);
    catto_addFunction(context, "chr", &catto_function_chr);
    catto_addFunction(context, "bin", &catto_function_bin);
    catto_addFunction(context, "oct", &catto_function_oct);
    catto_addFunction(context, "hex", &catto_function_hex);
    catto_addFunction(context, "len", &catto_function_len);
    catto_addFunction(context, "last", &catto_function_last);
    catto_addFunction(context, "split", &catto_function_split);
    catto_addFunction(context, "join", &catto_function_join);
    catto_addFunction(context, "find", &catto_function_find);
    catto_addFunction(context, "lower", &catto_function_lower);
    catto_addFunction(context, "upper", &catto_function_upper);
    catto_addFunction(context, "left", &catto_function_left);
    catto_addFunction(context, "right", &catto_function_right);
    catto_addFunction(context, "mid", &catto_function_mid);
    catto_addFunction(context, "trim", &catto_function_trim);
    catto_addFunction(context, "ltrim", &catto_function_ltrim);
    catto_addFunction(context, "rtrim", &catto_function_rtrim);

    // Constants

    catto_setVariable(context, "true", catto_asTypedNumber(1));
    catto_setVariable(context, "false", catto_asTypedNumber(0));
    catto_setVariable(context, "pi", catto_asTypedNumber(CATTO_PI));
    catto_setVariable(context, "e", catto_asTypedNumber(CATTO_E));
    catto_setVariable(context, "phi", catto_asTypedNumber(CATTO_PHI));
}

#endif
