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

    catto_AstNode* astNode = catto_addAstNode(CATTO_AST_NODE_TYPE_EXPRESSION_LEAF, currentAstNodePtr);

    // TODO: Store leaf node's value

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

    while (currentAstNode) {
        switch (currentAstNode->type) {
            case CATTO_AST_NODE_TYPE_COMMAND_STATEMENT:
                CATTO_LOG(currentAstNode->value.asStatement.attributes.asCommandName);
                CATTO_LOG_CHAR('(');

                catto_debugAstNodes(currentAstNode->value.asStatement.firstArgument);

                CATTO_LOG_CHAR(')');

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