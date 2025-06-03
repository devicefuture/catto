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
    catto_Count pointersToGcCount;
    catto_ErrorState errorState;
    catto_Count subjectLineNumber;
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
    catto_TypedValue value;
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

catto_Context* catto_newContext();
void catto_freeContext(catto_Context* context);
void catto_addPointerToGc(catto_Context* context, void* ptr);
void catto_removePointerFromGc(catto_Context* context, void* ptr);
void catto_gc(catto_Context* context);
void catto_addCommand(catto_Context* context, const catto_Char* name, catto_CommandHandlerFunction function);
void catto_addFunction(catto_Context* context, const catto_Char* name, catto_FunctionHandlerFunction function);
catto_DataType catto_removeTypeFromVariableName(catto_Char* name);
catto_TypedValue* catto_getVariable(catto_Context* context, catto_Char* name);
catto_Procedure* catto_getProcedure(catto_Context* context, const catto_Char* name);
catto_Procedure* catto_createProcedure(catto_Context* context, const catto_Char* name);
void catto_setVariable(catto_Context* context, const catto_Char* name, catto_TypedValue value);
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

catto_Float catto_power(catto_Float base, catto_Int power);
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

catto_List* catto_newList();
catto_List* catto_referenceList(catto_List* list);
void catto_destroyList(catto_Context* context, catto_List* list);
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