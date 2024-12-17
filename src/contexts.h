catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->firstCommandHandler = CATTO_NULL;
    context->lastCommandHandler = CATTO_NULL;
    context->firstVariable = CATTO_NULL;
    context->lastVariable = CATTO_NULL;

    context->firstParsedStatement = CATTO_NULL;
    context->nextParsedStatement = CATTO_NULL;
    context->firstParsedArgument = CATTO_NULL;
    context->nextParsedArgument = CATTO_NULL;

    context->pointersToGc = CATTO_MALLOC(0);
    context->pointersToGcCount = 0;

    context->errorState = CATTO_ERROR_STATE_NONE;
    context->subjectLineNumber = 0;

    return context;
}

void catto_addPointerToGc(catto_Context* context, void* ptr) {
    catto_removePointerFromGc(context, ptr);

    context->pointersToGc = CATTO_REALLOC(context->pointersToGc, sizeof(void*) * context->pointersToGcCount + 1);
    context->pointersToGc[context->pointersToGcCount++] = ptr;
}

void catto_removePointerFromGc(catto_Context* context, void* ptr) {
    if (!ptr || context->pointersToGcCount == 0) {
        return;
    }

    for (catto_Count i = 0; i < context->pointersToGcCount; i++) {
        if (context->pointersToGc[i] == ptr) {
            context->pointersToGc[i] = CATTO_NULL;
        }
    }
}

void catto_gc(catto_Context* context) {
    if (context->pointersToGcCount == 0) {
        return;
    }

    for (catto_Count i = 0; i < context->pointersToGcCount; i++) {
        void* ptr = context->pointersToGc[i];

        if (ptr) {
            CATTO_FREE(ptr);
        }
    }

    context->pointersToGc = CATTO_REALLOC(context->pointersToGc, 0);
    context->pointersToGcCount = 0;
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
    }

    context->lastCommandHandler = commandHandler;
}

catto_TypedValue* catto_getVariable(catto_Context* context, catto_Char* name) {
    catto_Variable* currentVariable = context->firstVariable;

    while (currentVariable) {
        if (catto_stringsEqual(currentVariable->name, name)) {
            return &(currentVariable->value);
        }

        currentVariable = currentVariable->nextVariable;
    }

    return CATTO_NULL;
}

void catto_setVariable(catto_Context* context, catto_Char* name, catto_TypedValue value) {
    catto_TypedValue* existingVariableValue = catto_getVariable(context, name);

    catto_removeTypedValueFromGc(context, value);

    if (existingVariableValue) {
        catto_addTypedValueToGc(context, *existingVariableValue);

        *existingVariableValue = value;
    } else {
        catto_Variable* variable = CATTO_NEW(catto_Variable);

        variable->name = catto_copyString(name);
        variable->value = value;
        variable->nextVariable = CATTO_NULL;

        if (!context->firstVariable) {
            context->firstVariable = variable;
        }
        
        if (context->lastVariable) {
            context->lastVariable->nextVariable = variable;
        }

        context->lastVariable = variable;
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

        catto_Char* subjectVariable = astNode->value.asExpressionLeaf.subjectVariable;

        if (subjectVariable) {
            catto_TypedValue* variableValue = catto_getVariable(context, subjectVariable);

            if (variableValue) {
                return *variableValue;
            }
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
            catto_Char* operator = astNode->value.asBinaryExpression.operators[i++];
            catto_Bool foundOperatorMapping = CATTO_FALSE;
            catto_Count j = 0;

            while (catto_operatorMappings[j].operator) {
                catto_OperatorMapping currentOperatorMapping = catto_operatorMappings[j];

                if (catto_stringsEqual(operator, currentOperatorMapping.operator)) {
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

catto_AstNode* catto_getNextArg(catto_Context* context) {
    catto_AstNode* currentArgument = context->nextParsedArgument;

    context->nextParsedArgument = currentArgument ? currentArgument->nextAstNode : CATTO_NULL;

    return currentArgument;
}

catto_TypedValue catto_evalNextArg(catto_Context* context) {
    return catto_evalExpression(context, catto_getNextArg(context));
}

catto_Bool catto_step(catto_Context* context) {
    if (!context->nextParsedStatement) {
        return CATTO_FALSE;
    }

    catto_AstNode* currentStatement = context->nextParsedStatement;

    context->currentParsedStatement = currentStatement;
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

        case CATTO_AST_NODE_TYPE_ASSIGNMENT_STATEMENT:
            catto_Char* variableName = currentStatement->value.asStatement.attributes.asAssignee.subjectVariable;
            catto_TypedValue value = catto_evalExpression(context, currentStatement->value.asStatement.firstArgument);

            catto_setVariable(context, variableName, value);

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

    catto_freeTokens(firstToken);
}

void catto_run(catto_Context* context) {
    while (CATTO_TRUE) {
        catto_AstNode* statement = context->nextParsedStatement;

        if (statement && statement->value.asStatement.lineNumber > 0) {
            context->subjectLineNumber = statement->value.asStatement.lineNumber;
        }

        if (!catto_step(context)) {
            break;
        }

        catto_gc(context);

        if (context->errorState != CATTO_ERROR_STATE_NONE) {
            break;
        }
    }
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

void catto_command_if(catto_Context* context) {
    catto_Bool isTrue = catto_asBool(catto_evalNextArg(context));

    catto_AstNode* elseStatement = catto_findClosingMark(context->currentParsedStatement, "else");
    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end");

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

void catto_command_else(catto_Context* context) {
    catto_Bool shouldSkip = catto_asBool(catto_evalNextArg(context));
    catto_Bool isElseIf = catto_hasNextArg(context);

    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end");

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

    catto_AstNode* elseStatement = catto_findClosingMark(context->currentParsedStatement, "else");

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

void catto_command_end(catto_Context* context) {}

void catto_command_for(catto_Context* context) {
    if (!catto_findClosingMark(context->currentParsedStatement, "next")) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    catto_AstNode* identifier = catto_getNextArg(context);
    catto_TypedValue start = catto_evalNextArg(context);

    if (identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_setVariable(context, identifier->value.asExpressionLeaf.subjectVariable, start);
}

void catto_command_next(catto_Context* context) {
    catto_AstNode* forStatement = catto_findOpeningMark(context->currentParsedStatement, "for");

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

    catto_setVariable(context, identifier->value.asExpressionLeaf.subjectVariable, catto_asTypedNumber(currentValue + catto_asNumber(step)));

    context->nextParsedStatement = forStatement->nextAstNode;
}

void catto_command_repeat(catto_Context* context) {
    catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while");
    catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until");
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop");

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

void catto_command_whileOrUntil(catto_Context* context, catto_Bool isUntil) {
    catto_Bool isClosing = catto_asBool(catto_evalNextArg(context));
    catto_Bool isTrue = catto_asBool(catto_evalNextArg(context));

    if (isUntil) {
        isTrue = !isTrue;
    }

    if (isClosing) {
        if (isTrue) {
            catto_AstNode* repeatStatement = catto_findOpeningMark(context->currentParsedStatement, "repeat");

            if (repeatStatement) {
                context->nextParsedStatement = repeatStatement;
            } else {
                context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
            }
        }

        return;
    }

    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop");

    if (!loopStatement) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    if (!isTrue) {
        context->nextParsedStatement = loopStatement->nextAstNode;
        return;
    }
}

void catto_command_while(catto_Context* context) {
    catto_command_whileOrUntil(context, CATTO_FALSE);
}

void catto_command_until(catto_Context* context) {
    catto_command_whileOrUntil(context, CATTO_TRUE);
}

void catto_command_loop(catto_Context* context) {
    catto_AstNode* repeatStatement = catto_findOpeningMark(context->currentParsedStatement, "repeat");
    catto_AstNode* whileStatement = catto_findOpeningMark(context->currentParsedStatement, "while");
    catto_AstNode* untilStatement = catto_findOpeningMark(context->currentParsedStatement, "until");

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

void catto_addContextStandardCommands(catto_Context* context) {
    catto_addCommand(context, "print", &catto_command_print);
    catto_addCommand(context, "goto", &catto_command_goto);
    catto_addCommand(context, "if", &catto_command_if);
    catto_addCommand(context, "else", &catto_command_else);
    catto_addCommand(context, "end", &catto_command_end);
    catto_addCommand(context, "for", &catto_command_for);
    catto_addCommand(context, "next", &catto_command_next);
    catto_addCommand(context, "repeat", &catto_command_repeat);
    catto_addCommand(context, "while", &catto_command_while);
    catto_addCommand(context, "until", &catto_command_until);
    catto_addCommand(context, "loop", &catto_command_loop);
}