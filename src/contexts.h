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

void catto_addContextStandardCommands(catto_Context* context) {
    catto_addCommand(context, "print", &catto_command_print);
}