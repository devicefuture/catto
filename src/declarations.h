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
    CATTO_TOKEN_TYPE_CLOSING_BRACKET = ')'
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
catto_Bool catto_stringStartsWith(catto_Char* a, catto_Char* b);
catto_Float catto_stringToPositiveNumber(catto_Char* string, catto_Count* charactersEaten);

catto_Token* catto_tokenise(catto_Char* code);
void catto_debugTokens(catto_Token* firstToken);

catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parse(catto_Token* firstToken);
void catto_debugAstNodes(catto_AstNode* firstAstNode);