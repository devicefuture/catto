typedef enum {
    CATTO_ERROR_STATE_NONE = 0,
    CATTO_ERROR_STATE_UNEXPECTED_TOKEN,
    CATTO_ERROR_STATE_INVALID_AT_FORMAT,
    CATTO_ERROR_STATE_NO_RETURN,
    CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK,
    CATTO_ERROR_STATE_MISMATCHED_CLOSING_MARK,
    CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP,
    CATTO_ERROR_STATE_EXTENSION_NOT_LOADED,
    CATTO_ERROR_STATE_UNKNOWN_EXTENSION,
    CATTO_ERROR_STATE_UNKNOWN_EXTENSION_COMMAND,
    CATTO_ERROR_STATE_UNKNOWN_PROCEDURE,
    CATTO_ERROR_STATE_NOT_A_FUNCTION,
    CATTO_ERROR_STATE_NOT_A_LIST,
    CATTO_ERROR_STATE_INVALID_LIST_VALUE,
    CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE,
    CATTO_ERROR_STATE_UNKNOWN_FIELD,
    CATTO_ERROR_STATE_OUT_OF_MEMORY
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
    struct cattox_Extension* firstExtension;
    struct cattox_Extension* lastExtension;
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
    catto_Bool shouldUseDefinedLineNumbers;
    catto_Bool scrawlMode;
    catto_TrigMode trigMode;
    catto_Count randomSeed;
    catto_Bool generatingTokenFile;
    catto_Char** tokenDefinitions;
    catto_Count tokenDefinitionsCount;
    catto_Count* tokenIndexes;
    catto_Count tokenIndexesCount;
    catto_Char* tokenFile;
    catto_Count tokenFileSize;
    void* userData;
} catto_Context;

typedef void (*catto_CommandHandlerFunction)(catto_Context* context);
typedef struct catto_TypedValue (*catto_FunctionHandlerFunction)(catto_Context* context, catto_DataType returnType);

typedef struct catto_CommandHandler {
    const catto_Char* name;
    catto_CommandHandlerFunction function;
    struct catto_CommandHandler* nextCommandHandler;
} catto_CommandHandler;

typedef struct cattox_Extension {
    const catto_Char* name;
    catto_Char* alias;
    catto_CommandHandler* firstCommandHandler;
    catto_CommandHandler* lastCommandHandler;
    struct cattox_Extension* nextExtension;
} cattox_Extension;

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
    CATTO_TOKEN_TYPE_CLOSING_ACCESSOR_BRACKET = ']',
    CATTO_TOKEN_TYPE_FIELD_ACCESSOR = '.'
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
    catto_Char** fields;
    catto_Count fieldCount;
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
    CATTO_AST_NODE_TYPE_EXTENSION_COMMAND_STATEMENT = 'x',
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
                    catto_Char* extensionName;
                    catto_Char* commandName;
                } asExtensionCommand;
                struct {
                    catto_Char* subjectVariable;
                    struct catto_AstNode* index;
                    catto_Char* field;
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
            catto_Char* field;
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

void catto_copyMemory(const catto_Char* source, catto_Char* destination, catto_Count length, catto_Count offset);
catto_Bool catto_memoryEquals(const catto_Char* a, const catto_Char* b, catto_Count length);

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
catto_Char* catto_getIdentifierName(catto_AstNode* astNode);
catto_Bool catto_hasNextArg(catto_Context* context);
catto_TypedValue catto_evalExpression(catto_Context* context, catto_AstNode* astNode);
catto_AstNode* catto_getNextArg(catto_Context* context);
catto_TypedValue catto_evalNextArg(catto_Context* context);
catto_AstNode* catto_getPenultimateArg(catto_Context* context);
catto_TypedValue catto_evalPenultimateArg(catto_Context* context);
catto_AstNode* catto_getLastArg(catto_Context* context);
catto_TypedValue catto_evalLastArg(catto_Context* context);
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
void catto_addListField(catto_List* list, catto_Char* field);
catto_List* catto_referenceList(catto_List* list);
void catto_dereferenceList(catto_Context* context, catto_List* list);
void catto_freeList(catto_Context* context, catto_List* list);
void catto_pushOntoList(catto_List* list, catto_TypedValue value);
catto_TypedValue catto_popFromList(catto_Context* context, catto_List* list);
void catto_insertIntoList(catto_List* list, catto_TypedValue value, catto_Count index);
catto_TypedValue catto_removeFromList(catto_Context* context, catto_List* list, catto_Count index);
catto_Count catto_getFlatIndex(catto_List* list, catto_Count index);
catto_Count catto_getFieldOffset(catto_List* list, catto_Char* field, catto_Bool* exists);
catto_TypedValue catto_getListItem(catto_List* list, catto_Count index);
void catto_setListItem(catto_Context* context, catto_List* list, catto_Count index, catto_TypedValue value);
catto_Char* catto_listToString(catto_List* list);

catto_Float catto_asNumber(catto_TypedValue value);
catto_TypedValue catto_asTypedNumber(catto_Float value);
catto_Char* catto_asString(catto_TypedValue value);
catto_TypedValue catto_asTypedString(const catto_Char* value);
catto_Bool catto_asBool(catto_TypedValue value);
void catto_freeTypedValue(catto_TypedValue* valuePtr);
catto_TypedValue catto_copyTypedValue(catto_TypedValue value);
catto_TypedValue catto_castTypedValue(catto_TypedValue value, catto_DataType type);
void catto_addTypedValueToGc(catto_Context* context, catto_TypedValue value);
void catto_removeTypedValueFromGc(catto_Context* context, catto_TypedValue value);

catto_Token* catto_tokenise(catto_Context* context, const catto_Char* code);
void catto_freeTokens(catto_Token* firstToken);
void catto_debugTokens(catto_Token* firstToken);

void catto_generateTokenFile(catto_Context* context);
catto_Bool catto_isTokenFile(catto_Context* context);
catto_Char* catto_parseTokenFile(catto_Context* context);

catto_AstNode* catto_createExpressionLeaf(catto_TypedValue value, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parseExpression(catto_Token** currentTokenPtr, catto_AstNode** currentAstNodePtr);
catto_AstNode* catto_parse(catto_Context* context, catto_Token* firstToken);
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

cattox_Extension* cattox_newExtension(catto_Context* context, const catto_Char* name);
cattox_Extension* cattox_findExtension(catto_Context* context, const catto_Char* name, catto_Bool useAlias);
catto_CommandHandler* cattox_findCommandHandlerInExtension(cattox_Extension* extension, const catto_Char* command);
void cattox_addExtensionCommand(cattox_Extension* extension, const catto_Char* name, catto_CommandHandlerFunction function);