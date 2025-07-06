catto_Context* catto_newContext() {
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
    context->randomSeed = 0;

    return context;
}

void catto_freeContext(catto_Context* context) {
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

void catto_addPointerToGc(catto_Context* context, catto_DataType type, void* ptr) {
    catto_removePointerFromGc(context, ptr);

    context->pointersToGc = (void**)CATTO_REALLOC(context->pointersToGc, sizeof(void*) * (context->pointersToGcCount + 1));
    context->pointersToGc[context->pointersToGcCount] = ptr;

    context->pointerTypesToGc = (catto_DataType*)CATTO_REALLOC(context->pointerTypesToGc, sizeof(catto_DataType) * (context->pointersToGcCount + 1));
    context->pointerTypesToGc[context->pointersToGcCount] = type;

    context->pointersToGcCount++;
}

void catto_removePointerFromGc(catto_Context* context, void* ptr) {
    if (!ptr || context->pointersToGcCount == 0) {
        return;
    }

    for (catto_Count i = 0; i < context->pointersToGcCount; i++) {
        if (context->pointersToGc[i] == ptr) {
            context->pointerTypesToGc[i] = CATTO_NULL;
        }
    }
}

void catto_gc(catto_Context* context) {
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

void catto_addCommand(catto_Context* context, const catto_Char* name, catto_CommandHandlerFunction function) {
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

void catto_addFunction(catto_Context* context, const catto_Char* name, catto_FunctionHandlerFunction function) {
    catto_setVariable(context, name, (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_FUNCTION,
        .value = {.asFunction = function}
    });
}

catto_DataType catto_removeTypeFromVariableName(catto_Char* name) {
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

catto_Variable* catto_getVariableObject(catto_Context* context, catto_Char* name, catto_Bool allowOutsideScope) {
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

catto_TypedValue* catto_getVariable(catto_Context* context, catto_Char* name) {
    catto_Variable* variable = catto_getVariableObject(context, name, CATTO_TRUE);

    if (!variable) {
        return CATTO_NULL;
    }

    return &(variable->value);
}

catto_Variable* catto_setVariableScoped(catto_Context* context, const catto_Char* name, catto_TypedValue value, catto_Bool allowOutsideScope) {
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

catto_Variable* catto_setVariable(catto_Context* context, const catto_Char* name, catto_TypedValue value) {
    return catto_setVariableScoped(context, name, value, CATTO_TRUE);
}

catto_Procedure* catto_getProcedure(catto_Context* context, const catto_Char* name) {
    catto_Procedure* currentProcedure = context->firstProcedure;

    while (currentProcedure) {
        if (catto_stringsEqualCaseInsensitive(currentProcedure->name, name)) {
            return currentProcedure;
        }

        currentProcedure = currentProcedure->nextProcedure;
    }

    return CATTO_NULL;
}

catto_Procedure* catto_createProcedure(catto_Context* context, const catto_Char* name) {
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

void catto_assignValue(catto_Context* context, catto_AstNode* astNode, catto_TypedValue value) {
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
    catto_Char* field = astNode->value.asExpressionLeaf.field;
    catto_Bool fieldExists;

    if (indexAstNode) {
        catto_Int index = (catto_Int)catto_asNumber(catto_evalExpression(context, indexAstNode));
        catto_TypedValue variableValue = *catto_getVariable(context, name);

        if (variableValue.type != CATTO_DATA_TYPE_LIST) {
            context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

            return;
        }

        catto_List* list = variableValue.value.asList;

        while (index < 0) {
            index += list->length;
        }

        index = catto_getFlatIndex(list, index);
        index += catto_getFieldOffset(list, field, &fieldExists);

        if (field && !fieldExists) {
            context->errorState = CATTO_ERROR_STATE_UNKNOWN_FIELD;

            return;
        }

        catto_setListItem(context, list, index, catto_copyTypedValue(value));

        return;
    }

    catto_setVariable(context, name, value);
}

catto_Bool catto_hasNextArg(catto_Context* context) {
    return !!context->nextParsedArgument;
}

catto_TypedValue catto_evalExpression(catto_Context* context, catto_AstNode* astNode) {
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
                catto_Char* field = astNode->value.asExpressionLeaf.field;
                catto_Bool fieldExists;

                if (indexAstNode) {
                    catto_Int index = (catto_Int)catto_asNumber(catto_evalExpression(context, indexAstNode));

                    if (variableValue.type != CATTO_DATA_TYPE_LIST) {
                        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

                        return DEFAULT_RETURN_VALUE;
                    }

                    catto_List* list = variableValue.value.asList;

                    while (index < 0) {
                        index += list->length;
                    }

                    index = catto_getFlatIndex(list, index);
                    index += catto_getFieldOffset(list, field, &fieldExists);

                    if (field && !fieldExists) {
                        context->errorState = CATTO_ERROR_STATE_UNKNOWN_FIELD;

                        return DEFAULT_RETURN_VALUE;
                    }

                    variableValue = catto_getListItem(list, index);
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

catto_Bool catto_hasAppendFlag(catto_AstNode* astNode) {
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
            catto_Char* field = currentStatement->value.asStatement.attributes.asAssignee.field;
            catto_Bool fieldExists;
            catto_TypedValue value = catto_evalExpression(context, currentStatement->value.asStatement.firstArgument);

            if (indexAstNode) {
                catto_Int index = (catto_Int)catto_asNumber(catto_evalExpression(context, indexAstNode));
                catto_TypedValue variableValue = *catto_getVariable(context, variableName);

                if (variableValue.type != CATTO_DATA_TYPE_LIST) {
                    context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;

                    return CATTO_FALSE;
                }

                catto_List* list = variableValue.value.asList;

                while (index < 0) {
                    index += list->length;
                }

                index = catto_getFlatIndex(list, index);
                index += catto_getFieldOffset(list, field, &fieldExists);

                if (field && !fieldExists) {
                    context->errorState = CATTO_ERROR_STATE_UNKNOWN_FIELD;

                    return CATTO_FALSE;
                }

                catto_setListItem(context, list, index, value);

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

void catto_pushOntoStatementStack(catto_Context* context, catto_AstNode* statement) {
    context->statementStack = (catto_AstNode**)CATTO_REALLOC(context->statementStack, (++context->statementStackCount) * sizeof(catto_AstNode**));

    context->statementStack[context->statementStackCount - 1] = statement;
}

void catto_removeScopedVariables(catto_Context* context) {
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

catto_AstNode* catto_popFromStatementStack(catto_Context* context) {
    if (context->statementStackCount == 0) {
        return CATTO_NULL;
    }

    catto_removeScopedVariables(context);

    catto_Count scope = context->statementStackCount;
    catto_AstNode* lastStatement = context->statementStack[context->statementStackCount - 1];

    context->statementStack = (catto_AstNode**)CATTO_REALLOC(context->statementStack, (--context->statementStackCount) * sizeof(catto_AstNode**));

    return lastStatement;
}

void catto_load(catto_Context* context, const catto_Char* code) {
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

void catto_run(catto_Context* context) {
    while (catto_step(context)) {}
}