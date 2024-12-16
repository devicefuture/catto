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
    catto_Bool isInitialised;
} catto_Context;

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
                catto_Char* asCommandName;
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

catto_Context* catto_newContext();

catto_Count catto_stringLength(catto_Char* string);
catto_Bool catto_stringsEqual(catto_Char* a, catto_Char* b);
catto_Char* catto_copyString(catto_Char* string);
catto_Bool catto_stringStartsWith(catto_Char* a, catto_Char* b);
catto_Float catto_stringToPositiveNumber(catto_Char* string, catto_Count* charactersEaten);

catto_Token* catto_tokenise(catto_Char* code);
void catto_debugTokens(catto_Token* firstToken);

catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parse(catto_Token* firstToken);
void catto_debugAstNodes(catto_AstNode* firstAstNode);

// src/contexts.h

catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->isInitialised = CATTO_FALSE;

    return context;
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
catto_Float catto_stringToPositiveNumber(catto_Char* string, catto_Count* charactersEaten) {
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

#endif

// src/tokeniser.h

#ifndef CATTO_TOKENISER_H_
#define CATTO_TOKENISER_H_

catto_Char* operators[] = {
    "+", "-", "*", "/", "^", "div", "mod", "&", "|", "~",
    "!=", "<=", ">=", "=", "<", ">",
    "and", "or", "xor", "not",
    ";",
    CATTO_NULL
};

catto_Char* commands[] = {
    "print",
    CATTO_NULL
};

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
    catto_Float number = catto_stringToPositiveNumber(code + *indexPtr, &charactersEaten);

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

catto_Token* catto_tokenise(catto_Char* code) {
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

        if (catto_matchStrings(operators, CATTO_TOKEN_TYPE_OPERATOR, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchStrings(commands, CATTO_TOKEN_TYPE_COMMAND, code, &index, &currentToken)) {
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

catto_Char** operatorPrecedence[] = {
    (catto_Char*[]) {"+", "-", CATTO_NULL},
    (catto_Char*[]) {"*", "/", "div", "mod", CATTO_NULL},
    (catto_Char*[]) {"^", CATTO_NULL},
    (catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    (catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    (catto_Char*[]) {"!=", "<=", ">=", "=", "<", ">", CATTO_NULL},
    (catto_Char*[]) {"and", "or", "xor", CATTO_NULL},
    (catto_Char*[]) {";", CATTO_NULL},
    CATTO_NULL
};

catto_Char* unaryOperators[] = {
    "+",
    "-",
    "not",
    CATTO_NULL
};

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

    // TODO: Store leaf node's value

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
    catto_Char** operatorsAtLevel = operatorPrecedence[level];

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

    while (unaryOperators[i]) {
        if (catto_stringsEqual(unaryOperators[i], operator->value.asString)) {
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
        } else if (operatorPrecedence[operatorPrecedenceLevel + 1]) {
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

    astNode->value.asStatement.attributes.asCommandName = commandToken->value.asString;

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
                CATTO_LOG(currentAstNode->value.asStatement.attributes.asCommandName);
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
