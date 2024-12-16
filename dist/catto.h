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

#define CATTO_MAX_PRECISION 6

#endif

#define CATTO_USE_STDLIB

#ifdef CATTO_USE_STDLIB

#include <stdio.h>
#include <stdlib.h>

void _catto_log(char* text) {
    printf("%s", text);
}

void _catto_logChar(char character) {
    printf("%c", character);
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

#define CATTO_NEW(type) (type*)CATTO_MALLOC(sizeof(type))

// src/declarations.h

typedef struct catto_Context {
    struct catto_CommandHandler* firstCommandHandler;
    struct catto_CommandHandler* lastCommandHandler;
    struct catto_AstNode* firstParsedStatement;
    struct catto_AstNode* nextParsedStatement;
    struct catto_AstNode* firstParsedArgument;
    struct catto_AstNode* nextParsedArgument;
} catto_Context;

typedef void (*catto_CommandHandlerFunction)(catto_Context* context);

typedef struct catto_CommandHandler {
    catto_Char* name;
    catto_CommandHandlerFunction function;
    struct catto_CommandHandler* nextCommandHandler;
} catto_CommandHandler;

typedef enum {
    CATTO_TOKEN_TYPE_SYNTAX_ERROR = '\0',
    CATTO_TOKEN_TYPE_NEXT_LINE = 'N',
    CATTO_TOKEN_TYPE_LINE_NUMBER = 'L',
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
        catto_CommandHandler* asCommandHandler;
    } value;
    struct catto_Token* nextToken;
} catto_Token;

typedef enum {
    CATTO_DATA_TYPE_NUMBER = '%',
    CATTO_DATA_TYPE_STRING = '$'
} catto_DataType;

typedef struct catto_TypedValue {
    catto_DataType type;
    union {
        catto_Float asNumber;
        catto_Char* asString;
    } value;
} catto_TypedValue;

typedef enum {
    CATTO_AST_NODE_TYPE_SYNTAX_ERROR = '\0',
    CATTO_AST_NODE_TYPE_COMMAND_STATEMENT = 'c',
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
            } attributes;
        } asStatement;
        struct {
            catto_TypedValue* value;
            catto_Char* subjectVariable;
            struct catto_AstNode* index;
        } asExpressionLeaf;
        struct {
            struct catto_AstNode* child;
            catto_Char* operator;
        } asUnaryExpression;
        struct {
            struct catto_AstNode* firstChild;
            catto_Char** operators;
        } asBinaryExpression;
    } value;
    struct catto_AstNode* nextAstNode;
} catto_AstNode;

typedef catto_TypedValue (*catto_UnaryOperatorFunction)(catto_TypedValue value);
typedef catto_TypedValue (*catto_BinaryOperatorFunction)(catto_TypedValue a, catto_TypedValue b);

typedef struct catto_OperatorMapping {
    catto_Char* operator;
    catto_UnaryOperatorFunction unaryFunction;
    catto_BinaryOperatorFunction binaryFunction;
} catto_OperatorMapping;

catto_Context* catto_newContext();
void catto_addCommand(catto_Context* context, catto_Char* name, catto_CommandHandlerFunction function);
void catto_addContextStandardCommands(catto_Context* context);

catto_Float catto_power(catto_Float base, catto_Int power);
catto_Float catto_roundToPrecision(catto_Float number, catto_Count precision);
catto_Char* catto_numberToString(catto_Float number);

catto_Count catto_stringLength(catto_Char* string);
catto_Bool catto_stringsEqual(catto_Char* a, catto_Char* b);
catto_Char* catto_copyString(catto_Char* string);
catto_Char* catto_appendCharToString(catto_Char* string, catto_Char character);
catto_Char* catto_appendToString(catto_Char* a, catto_Char* b);
catto_Char* catto_reverseString(catto_Char* string);
catto_Bool catto_stringStartsWith(catto_Char* a, catto_Char* b);
catto_Float catto_unsignedStringToNumber(catto_Char* string, catto_Count* charactersEaten);

catto_Float catto_asNumber(catto_TypedValue value);
catto_TypedValue catto_asTypedNumber(catto_Float value);
catto_Char* catto_asString(catto_TypedValue value);
catto_TypedValue catto_asTypedString(catto_Char* value);

catto_Token* catto_tokenise(catto_Context* context, catto_Char* code);
void catto_debugTokens(catto_Token* firstToken);

catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parse(catto_Token* firstToken);
void catto_debugAstNodes(catto_AstNode* firstAstNode);

// src/operators.h

catto_Char* catto_operators[] = {
    "+", "-", "*", "/", "div", "mod", "^", "&", "|", "~",
    "!=", "<=", ">=", "=", "<", ">",
    "and", "or", "xor", "not",
    ";",
    CATTO_NULL
};

catto_Char** catto_operatorPrecedence[] = {
    (catto_Char*[]) {"and", "or", "xor", CATTO_NULL},
    (catto_Char*[]) {"!=", "<=", ">=", "=", "<", ">", CATTO_NULL},
    (catto_Char*[]) {"+", "-", CATTO_NULL},
    (catto_Char*[]) {"*", "/", "div", "mod", CATTO_NULL},
    (catto_Char*[]) {"^", CATTO_NULL},
    (catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    (catto_Char*[]) {";", CATTO_NULL},
    CATTO_NULL
};

catto_Char* catto_unaryOperators[] = {
    "+",
    "-",
    "not",
    CATTO_NULL
};

#define CATTO_UNARY_NUMERIC_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue value) { \
        return catto_asTypedNumber(operator catto_asNumber(value)); \
    }

#define CATTO_UNARY_INTEGER_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue value) { \
        return catto_asTypedNumber(operator (catto_Int)catto_asNumber(value)); \
    }

#define CATTO_BINARY_NUMERIC_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber(catto_asNumber(a) operator catto_asNumber(b)); \
    }

#define CATTO_BINARY_INTEGER_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber((catto_Int)catto_asNumber(a) operator (catto_Int)catto_asNumber(b)); \
    }

#define CATTO_BINARY_LOGICAL_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue a, catto_TypedValue b) { \
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
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_notEqual, !=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_lessThanOrEqual, <=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_greaterThanOrEqual, >=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_equal, ==);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_lessThan, <);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_greaterThan, >);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_and, &&);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_or, ||);

catto_TypedValue catto_binary_power(catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(catto_power(catto_asNumber(a), (catto_Int)catto_asNumber(b)));
}

catto_TypedValue catto_binary_xor(catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(((catto_Int)catto_asNumber(a) ^ (catto_Int)catto_asNumber(b)) ? 1 : 0);
}

catto_OperatorMapping catto_operatorMappings[] = {
    {"+", catto_unary_add, catto_binary_add},
    {"-", catto_unary_subtract, catto_binary_subtract},
    {"*", CATTO_NULL, catto_binary_multiply},
    {"/", CATTO_NULL, catto_binary_divide},
    {"div", CATTO_NULL, catto_binary_integerDivide},
    {"mod", CATTO_NULL, catto_binary_modulo},
    {"^", CATTO_NULL, catto_binary_power},
    {"&", CATTO_NULL, catto_binary_bitwiseAnd},
    {"|", CATTO_NULL, catto_binary_bitwiseOr},
    {"~", catto_unary_bitwiseNot, CATTO_NULL},
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
    {CATTO_NULL, CATTO_NULL, CATTO_NULL}
};

// src/contexts.h

catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->firstCommandHandler = CATTO_NULL;
    context->lastCommandHandler = CATTO_NULL;
    context->firstParsedStatement = CATTO_NULL;
    context->nextParsedStatement = CATTO_NULL;
    context->firstParsedArgument = CATTO_NULL;
    context->nextParsedArgument = CATTO_NULL;

    return context;
}

void catto_addCommand(catto_Context* context, catto_Char* name, catto_CommandHandlerFunction function) {
    catto_CommandHandler* commandHandler = CATTO_NEW(catto_CommandHandler);

    commandHandler->name = name;
    commandHandler->function = function;
    commandHandler->nextCommandHandler = CATTO_NULL;

    if (!context->firstCommandHandler) {
        context->firstCommandHandler = commandHandler;
    }

    if (context->lastCommandHandler) {
        context->lastCommandHandler->nextCommandHandler = commandHandler;
    } else {
        context->lastCommandHandler = commandHandler;
    }
}

catto_Bool catto_hasNextArg(catto_Context* context) {
    return !!context->nextParsedArgument;
}

catto_TypedValue catto_evalExpression(catto_Context* context, catto_AstNode* astNode) {
    const catto_TypedValue DEFAULT_RETURN_VALUE = (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_NUMBER,
        .value.asNumber = 0
    };

    if (!astNode) {
        return DEFAULT_RETURN_VALUE;
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        if (astNode->value.asExpressionLeaf.value) {
            return *(astNode->value.asExpressionLeaf.value);
        }
    }

    if (astNode->type == CATTO_AST_NODE_TYPE_UNARY_EXPRESSION) {
        catto_Char* operator = astNode->value.asUnaryExpression.operator;
        catto_Count i = 0;

        while (catto_operatorMappings[i].operator) {
            catto_OperatorMapping currentOperatorMapping = catto_operatorMappings[i];

            if (catto_stringsEqual(operator, currentOperatorMapping.operator)) {
                catto_UnaryOperatorFunction function = currentOperatorMapping.unaryFunction;

                if (function) {
                    return function(catto_evalExpression(context, astNode->value.asUnaryExpression.child));
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
            catto_Char* operator = astNode->value.asBinaryExpression.operators[i++];
            catto_Bool foundOperatorMapping = CATTO_FALSE;
            catto_Count j = 0;

            while (catto_operatorMappings[j].operator) {
                catto_OperatorMapping currentOperatorMapping = catto_operatorMappings[j];

                if (catto_stringsEqual(operator, currentOperatorMapping.operator)) {
                    catto_BinaryOperatorFunction function = currentOperatorMapping.binaryFunction;

                    if (function) {
                        currentValue = function(currentValue, catto_evalExpression(context, currentChild));
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

catto_TypedValue catto_evalNextArg(catto_Context* context) {
    catto_AstNode* currentArgument = context->nextParsedArgument;
    catto_TypedValue returnValue = catto_evalExpression(context, currentArgument);

    context->nextParsedArgument = currentArgument->nextAstNode;

    return returnValue;
}

catto_Bool catto_step(catto_Context* context) {
    if (!context->nextParsedStatement) {
        return CATTO_FALSE;
    }

    catto_AstNode* currentStatement = context->nextParsedStatement;

    context->nextParsedStatement = currentStatement->nextAstNode;
    context->firstParsedArgument = currentStatement->value.asStatement.firstArgument;
    context->nextParsedArgument = context->firstParsedArgument;

    switch (currentStatement->type) {
        case CATTO_AST_NODE_TYPE_COMMAND_STATEMENT:
            catto_CommandHandler* commandHandler = currentStatement->value.asStatement.attributes.asCommandHandler;

            if (!commandHandler) {
                return CATTO_FALSE;
            }

            catto_CommandHandlerFunction function = commandHandler->function;

            if (!function) {
                return CATTO_FALSE;
            }

            function(context);

            break;

        default:
            return CATTO_FALSE;
    }

    return !!context->nextParsedStatement;
}

void catto_goto(catto_Context* context, catto_Count lineNumber) {
    catto_AstNode* currentStatement = context->firstParsedStatement;

    while (currentStatement) {
        if (currentStatement->value.asStatement.lineNumber >= lineNumber) {
            break;
        }

        currentStatement = currentStatement->nextAstNode;
    }

    context->nextParsedStatement = currentStatement;
}

void catto_load(catto_Context* context, catto_Char* code) {
    catto_Token* firstToken = catto_tokenise(context, code);
    catto_AstNode* firstAstNode = catto_parse(firstToken);

    context->firstParsedStatement = firstAstNode;
    context->nextParsedStatement = firstAstNode;
}

void catto_run(catto_Context* context) {
    while (catto_step(context)) {}
}

void catto_command_print(catto_Context* context) {
    while (catto_hasNextArg(context)) {
        catto_Char* string = catto_asString(catto_evalNextArg(context));

        CATTO_LOG(string);
        CATTO_FREE(string);

        if (catto_hasNextArg(context)) {
            CATTO_LOG(" ");
        }
    }

    CATTO_LOG("\n");
}

void catto_command_goto(catto_Context* context) {
    catto_Int lineNumber = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (lineNumber < 0) {
        lineNumber = 0;
    }

    catto_goto(context, lineNumber);
}

void catto_addContextStandardCommands(catto_Context* context) {
    catto_addCommand(context, "print", &catto_command_print);
    catto_addCommand(context, "goto", &catto_command_goto);
}

// src/numbers.h

catto_Float catto_power(catto_Float base, catto_Int power) {
    if (power == 0) {
        return 1;
    }

    if (power < 0) {
        base = 1 / base;
        power *= -1;
    }

    catto_Float result = base;

    while (power > 1) {
        result *= base;
        power--;
    }

    return result;
}

catto_Float catto_roundToPrecision(catto_Float number, catto_Count precision) {
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

    number += 0.5 * catto_power(10, -precision);

    if (isNegative) {
        number *= -1;
    }

    return (catto_Float)((catto_Int)(number * multiplier)) / multiplier;
}

catto_Char* catto_numberToString(catto_Float number) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    if (number == CATTO_NAN) {
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

    number += 0.1 * catto_power(10, -precisionLeft);
    number -= integralPart; // Now fractional part

    do {
        catto_appendCharToString(string, (catto_Char)('0' + (integralPart % 10)));

        integralPart /= 10;
        precisionLeft--;
    } while (integralPart > 0);

    if (isNegative) {
        catto_appendCharToString(string, '-');
    }

    catto_reverseString(string);

    catto_Count trailingZeroes = 0;
    catto_Bool anyDigitsInFractionalPart = CATTO_FALSE;

    if (number > 0 && precisionLeft > 0) {
        catto_appendCharToString(string, '.');

        while (number > 0 && precisionLeft > 0) {
            number *= 10;

            catto_Char digit = number;

            if (digit == 0) {
                trailingZeroes++;
            } else {
                trailingZeroes = 0;
                anyDigitsInFractionalPart = CATTO_TRUE;
            }

            catto_appendCharToString(string, (catto_Char)('0' + digit));

            number -= digit;
            precisionLeft--;
        }
    }

    if (trailingZeroes > 0) {
        if (!anyDigitsInFractionalPart) {
            trailingZeroes++;
        }

        catto_Count newStringLength = catto_stringLength(string) - trailingZeroes;

        string = CATTO_REALLOC(string, newStringLength + 1);
        string[newStringLength] = '\0';
    }

    if (exponent != 0) {
        catto_appendCharToString(string, 'E');

        if (exponent > 0) {
            catto_appendCharToString(string, '+');
        }

        catto_Char* exponentString = catto_numberToString(exponent);

        string = catto_appendToString(string, exponentString);

        CATTO_FREE(exponentString);
    }

    return string;
}

// src/strings.h

#ifndef CATTO_STRINGS_H_
#define CATTO_STRINGS_H_

catto_Count catto_stringLength(catto_Char* string) {
    catto_Count length = 0;

    while (string[length] != '\0') {
        length++;
    }

    return length;
}

catto_Bool catto_stringsEqual(catto_Char* a, catto_Char* b) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (a[i] == b[i]) {
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

catto_Char* catto_copyString(catto_Char* string) {
    catto_Count length = catto_stringLength(string);
    catto_Char* newString = CATTO_MALLOC(length + 1);

    for (catto_Count i = 0; i < length; i++) {
        newString[i] = string[i];
    }

    newString[length] = '\0';

    return newString;
}

catto_Char* catto_appendCharToString(catto_Char* string, catto_Char character) {
    catto_Count length = catto_stringLength(string);

    string = CATTO_REALLOC(string, length + 2);
    string[length] = character;
    string[length + 1] = '\0';

    return string;
}

catto_Char* catto_appendToString(catto_Char* a, catto_Char* b) {
    catto_Count aLength = catto_stringLength(a);
    catto_Count bLength = catto_stringLength(b);

    a = CATTO_REALLOC(a, aLength + bLength + 1);
    a[aLength + bLength] = '\0';

    for (catto_Count i = 0; i < bLength; i++) {
        a[aLength + i] = b[i];
    }

    return a;
}

catto_Char* catto_reverseString(catto_Char* string) {
    catto_Char* tempString = catto_copyString(string);
    catto_Count stringLength = catto_stringLength(string);

    for (catto_Count i = 0; i < stringLength; i++) {
        string[stringLength - 1 - i] = tempString[i];
    }

    CATTO_FREE(tempString);

    return string;
}

catto_Bool catto_stringStartsWith(catto_Char* a, catto_Char* b) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (a[i] == b[i]) {
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

// @source https://stackoverflow.com/a/4392789
catto_Float catto_unsignedStringToNumber(catto_Char* string, catto_Count* charactersEaten) {
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

catto_Float catto_stringToNumber(catto_Char* string, catto_Count* charactersEaten) {
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

#endif

// src/datatypes.h

catto_Float catto_asNumber(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return value.value.asNumber;
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_Count charactersEaten = 0;

        return catto_stringToNumber(value.value.asString, &charactersEaten);
    }

    return 0;
}

catto_TypedValue catto_asTypedNumber(catto_Float value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_NUMBER,
        .value.asNumber = value
    };
}

catto_Char* catto_asString(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return catto_numberToString(value.value.asNumber);
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        return catto_copyString(value.value.asString);
    }

    return catto_copyString("");
}

catto_TypedValue catto_asTypedString(catto_Char* value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_STRING,
        .value.asString = catto_copyString(value)
    };
}

// src/tokeniser.h

#ifndef CATTO_TOKENISER_H_
#define CATTO_TOKENISER_H_

catto_Token* catto_addToken(catto_TokenType type, catto_Token** currentTokenPtr) {
    catto_Token* token = CATTO_NEW(catto_Token);

    token->type = type;
    token->nextToken = CATTO_NULL;

    if (*currentTokenPtr) {
        (*currentTokenPtr)->nextToken = token;        
    }

    *currentTokenPtr = token;

    return token;
}

catto_Token* catto_matchLineNumber(catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
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

catto_Token* catto_matchChar(catto_Char matchChar, catto_TokenType type, catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    if (code[*indexPtr] != matchChar) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(type, currentTokenPtr);

    (*indexPtr)++;

    return token;
}

catto_Token* catto_matchCommand(catto_Context* context, catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_CommandHandler* currentCommandHandler = context->firstCommandHandler;

    while (currentCommandHandler) {
        if (catto_stringStartsWith(code + index, currentCommandHandler->name)) {
            catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_COMMAND, currentTokenPtr);

            token->value.asCommandHandler = currentCommandHandler;

            *indexPtr = index + catto_stringLength(currentCommandHandler->name);

            return token;
        }

        currentCommandHandler = currentCommandHandler->nextCommandHandler;
    }

    return CATTO_NULL;
}

catto_Token* catto_matchStrings(catto_Char** matchStrings, catto_TokenType type, catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_Count i = 0;

    while (matchStrings[i]) {
        catto_Char* currentString = matchStrings[i];

        if (catto_stringStartsWith(code + index, currentString)) {
            catto_Token* token = catto_addToken(type, currentTokenPtr);

            token->value.asString = currentString;

            *indexPtr = index + catto_stringLength(currentString);

            return token;
        }

        i++;
    }

    return CATTO_NULL;
}

catto_Token* catto_matchNumber(catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count charactersEaten = 0;
    catto_Float number = catto_unsignedStringToNumber(code + *indexPtr, &charactersEaten);

    if (number == CATTO_NAN) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_NUMBER, currentTokenPtr);

    token->value.asNumber = number;

    *indexPtr += charactersEaten;

    return token;
}

catto_Token* catto_matchStringLiteral(catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;

    catto_Char stringOpener = code[index++];
    catto_Char* currentString = CATTO_MALLOC(8);
    catto_Count currentStringIndex = 0;

    if (stringOpener != '"' && stringOpener != '\'' && stringOpener != '`') {
        return CATTO_NULL;
    }

    while (CATTO_TRUE) {
        catto_Char currentChar = code[index++];

        if (currentChar == stringOpener) {
            break;
        }

        if (currentChar == '\0' || currentChar == '\n') {
            return CATTO_NULL;
        }

        if (currentChar == '\'') {
            switch (code[index]) {
                case '\0':
                case '\n':
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

        if (currentStringIndex + 1 == sizeof(currentString)) {
            currentString = CATTO_REALLOC(currentString, currentStringIndex + 9);
        }
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_STRING, currentTokenPtr);

    token->value.asString = currentString;

    *indexPtr = index;

    return token;
}

catto_Token* catto_matchIdentifier(catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_Char* currentString = CATTO_MALLOC(8);
    catto_Count currentStringIndex = 0;
    catto_Char currentChar = code[index++];

    if (!(
        (currentChar >= 'a' && currentChar <= 'z') ||
        (currentChar >= 'A' && currentChar <= 'Z') ||
        currentChar == '_'
    )) {
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
            (currentChar >= '0' && currentChar <= '9') ||
            currentChar == '_'
        )) {
            index--;

            break;
        }

        currentString[currentStringIndex++] = currentChar;
        currentString[currentStringIndex] = '\0';

        if (currentStringIndex + 1 == sizeof(currentString)) {
            currentString = CATTO_REALLOC(currentString, currentStringIndex + 9);
        }
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_IDENTIFIER, currentTokenPtr);

    token->value.asString = currentString;

    *indexPtr = index;

    return token;
}

catto_Token* catto_tokenise(catto_Context* context, catto_Char* code) {
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

        if (catto_matchStrings(catto_operators, CATTO_TOKEN_TYPE_OPERATOR, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchCommand(context, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchStringLiteral(code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchIdentifier(code, &index, &currentToken)) {
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

void catto_debugTokens(catto_Token* firstToken) {
    catto_Token* currentToken = firstToken;

    while (currentToken) {
        CATTO_LOG_CHAR(currentToken->type);

        currentToken = currentToken->nextToken;
    }
}

#endif

// src/parser.h

catto_Token* catto_eat(catto_Token** currentTokenPtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Token* returnToken = *currentTokenPtr;

    *currentTokenPtr = returnToken->nextToken;
    
    return returnToken;
}

catto_Token* catto_eatIfType(catto_Token** currentTokenPtr, catto_TokenType type) {
    if (!*currentTokenPtr || (*currentTokenPtr)->type != type) {
        return CATTO_NULL;
    }

    catto_eat(currentTokenPtr);
}

catto_AstNode* catto_addAstNode(catto_AstNodeType type, catto_AstNode** currentAstNodePtr) {
    catto_AstNode* astNode = CATTO_NEW(catto_AstNode);

    astNode->type = type;
    astNode->nextAstNode = CATTO_NULL;

    if (*currentAstNodePtr) {
        (*currentAstNodePtr)->nextAstNode = astNode;
    }

    *currentAstNodePtr = astNode;

    return astNode;
}

catto_AstNode* catto_parseExpressionLeaf(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
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
    astNode->value.asExpressionLeaf.index = index;

    return astNode;
}

catto_Bool catto_matchesInOperatorPrecedenceLevel(catto_Token* token, catto_Count level) {
    catto_Char** operatorsAtLevel = catto_operatorPrecedence[level];

    if (!operatorsAtLevel || !token || token->type != CATTO_TOKEN_TYPE_OPERATOR) {
        return CATTO_FALSE;
    }

    catto_Count i = 0;

    while (operatorsAtLevel[i]) {
        if (catto_stringsEqual(operatorsAtLevel[i], token->value.asString)) {
            return CATTO_TRUE;
        }

        i++;
    }

    return CATTO_FALSE;
}

catto_AstNode* catto_parseUnaryExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Token* operator = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPERATOR);

    if (!operator) {
        return CATTO_NULL;
    }

    catto_Count i = 0;
    catto_Bool operatorIsUnary = CATTO_FALSE;

    while (catto_unaryOperators[i]) {
        if (catto_stringsEqual(catto_unaryOperators[i], operator->value.asString)) {
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
    astNode->value.asUnaryExpression.operator = operator->value.asString;

    return astNode;
}

catto_AstNode* catto_parseBinaryExpression(catto_Count operatorPrecedenceLevel, catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_Char** operators = CATTO_MALLOC(sizeof(catto_Char*));
    catto_Count operatorCount = 0;

    operators[0] = CATTO_NULL;

    catto_AstNode* firstChild = CATTO_NULL;
    catto_AstNode* currentChild = CATTO_NULL;

    while (CATTO_TRUE) {
        if (catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_OPENING_BRACKET)) {
            if (!catto_parseExpression(currentTokenPtr, &currentChild)) {
                return CATTO_NULL;
            }

            if (!catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_CLOSING_BRACKET)) {
                return CATTO_NULL;
            }
        } else if (*currentTokenPtr && (*currentTokenPtr)->type == CATTO_TOKEN_TYPE_OPERATOR) {
            if (!catto_parseUnaryExpression(currentTokenPtr, &currentChild)) {
                return CATTO_NULL;
            }
        } else if (catto_operatorPrecedence[operatorPrecedenceLevel + 1]) {
            if (!catto_parseBinaryExpression(operatorPrecedenceLevel + 1, currentTokenPtr, &currentChild)) {
                return CATTO_NULL;
            }
        } else {
            if (!catto_parseExpressionLeaf(currentTokenPtr, &currentChild)) {
                return CATTO_NULL;
            }
        }

        if (!firstChild) {
            firstChild = currentChild;
        }

        catto_Token* operator = *currentTokenPtr;

        if (!operator || operator->type != CATTO_TOKEN_TYPE_OPERATOR) {
            break;
        }

        catto_Count lowerOperatorPrecedenceLevel = operatorPrecedenceLevel;
        catto_Bool reachedLowerPrecedenceLevelOperator = CATTO_FALSE;

        while (lowerOperatorPrecedenceLevel > 0) {
            if (catto_matchesInOperatorPrecedenceLevel(operator, lowerOperatorPrecedenceLevel - 1)) {
                reachedLowerPrecedenceLevelOperator = CATTO_TRUE;
                break;
            }

            lowerOperatorPrecedenceLevel--;
        }

        if (reachedLowerPrecedenceLevelOperator) {
            break;
        }

        catto_eat(currentTokenPtr);

        operators = CATTO_REALLOC(operators, sizeof(catto_Char*) * (operatorCount + 2));
        operators[operatorCount++] = operator->value.asString;
        operators[operatorCount] = CATTO_NULL;
    }

    if (operatorCount == 0) {
        if (*currentAstNodePtr) {
            (*currentAstNodePtr)->nextAstNode = firstChild;
        }

        *currentAstNodePtr = firstChild;

        return firstChild;
    }

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_BINARY_EXPRESSION, currentAstNodePtr);

    astNode->value.asBinaryExpression.firstChild = firstChild;
    astNode->value.asBinaryExpression.operators = operators;

    return astNode;
}

catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    return catto_parseBinaryExpression(0, currentTokenPtr, currentAstNodePtr);
}

catto_AstNode* catto_parseStatement(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr) {
    if (!*currentTokenPtr) {
        return CATTO_NULL;
    }

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_COMMAND_STATEMENT, currentAstNodePtr);
    catto_Token* lineNumberToken = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_LINE_NUMBER);

    astNode->value.asStatement.lineNumber = lineNumberToken ? lineNumberToken->value.asLineNumber : 0;

    catto_Token* commandToken = catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_COMMAND);

    if (!commandToken) {
        return CATTO_NULL;
    }

    astNode->value.asStatement.attributes.asCommandHandler = commandToken->value.asCommandHandler;

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

    astNode->value.asStatement.firstArgument = firstArgument;

    return astNode;
}

catto_AstNode* catto_parse(catto_Token* firstToken) {
    catto_Token** currentTokenPtr = &firstToken;
    catto_AstNode* firstAstNode = CATTO_NULL;
    catto_AstNode* currentAstNode = CATTO_NULL;

    while (*currentTokenPtr) {
        if (currentAstNode && !firstAstNode) {
            firstAstNode = currentAstNode;
        }

        if (catto_parseStatement(currentTokenPtr, &currentAstNode)) {
            while (
                catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_STATEMENT_DELIMETER) ||
                catto_eatIfType(currentTokenPtr, CATTO_TOKEN_TYPE_NEXT_LINE)
            ) {}
        } else {
            catto_addAstNode(CATTO_AST_NODE_TYPE_SYNTAX_ERROR, &currentAstNode);

            break;
        }
    }

    return firstAstNode;
}

void catto_debugAstNodes(catto_AstNode* firstAstNode) {
    catto_AstNode* currentAstNode = firstAstNode;
    catto_Bool hadFirst = CATTO_FALSE;

    while (currentAstNode) {
        if (hadFirst) {
            CATTO_LOG_CHAR(' ');
        } else {
            hadFirst = CATTO_TRUE;
        }

        switch (currentAstNode->type) {
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

            case CATTO_AST_NODE_TYPE_EXPRESSION_LEAF:
                if (currentAstNode->value.asExpressionLeaf.subjectVariable) {
                    CATTO_LOG(currentAstNode->value.asExpressionLeaf.subjectVariable);
                } else if (currentAstNode->value.asExpressionLeaf.value) {
                    CATTO_LOG_CHAR(currentAstNode->value.asExpressionLeaf.value->type);
                } else {
                    CATTO_LOG_CHAR('e');
                }

                if (currentAstNode->value.asExpressionLeaf.index) {
                    CATTO_LOG_CHAR('[');

                    catto_debugAstNodes(currentAstNode->value.asExpressionLeaf.index);

                    CATTO_LOG_CHAR(']');
                }

                break;

            case CATTO_AST_NODE_TYPE_UNARY_EXPRESSION:
                CATTO_LOG(currentAstNode->value.asUnaryExpression.operator);
                CATTO_LOG_CHAR('(');

                catto_debugAstNodes(currentAstNode->value.asUnaryExpression.child);

                CATTO_LOG_CHAR(')');

                break;

            case CATTO_AST_NODE_TYPE_BINARY_EXPRESSION:
                CATTO_LOG(currentAstNode->value.asBinaryExpression.operators[0]);
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

#endif
