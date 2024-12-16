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