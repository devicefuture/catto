catto_Token* catto_eat(catto_Token** currentToken, catto_TokenType type) {
    if (!*currentToken || (*currentToken)->type != type) {
        return CATTO_NULL;
    }

    catto_Token* returnToken = *currentToken;

    *currentToken = returnToken->nextToken;
    
    return returnToken;
}

catto_AstNode* catto_parseStatement(catto_Token** currentToken) {
    if (!*currentToken) {
        return CATTO_NULL;
    }

    catto_AstNode* astNode = CATTO_NEW(catto_AstNode);
    catto_Token* lineNumberToken = catto_eat(currentToken, CATTO_TOKEN_TYPE_LINE_NUMBER);

    astNode->type = CATTO_AST_NODE_TYPE_COMMAND_STATEMENT;
    astNode->value.asStatement.lineNumber = lineNumberToken ? lineNumberToken->value.asLineNumber : 0;
    astNode->nextAstNode = CATTO_NULL;

    catto_Token* commandToken = catto_eat(currentToken, CATTO_TOKEN_TYPE_COMMAND);

    if (!commandToken) {
        return CATTO_NULL;
    }

    astNode->value.asStatement.firstArgument = CATTO_NULL;
    astNode->value.asStatement.attributes.asCommandName = commandToken->value.asString;

    return astNode;
}

catto_AstNode* catto_parse(catto_Token* firstToken) {
    catto_Token** currentToken = &firstToken;
    catto_AstNode* firstAstNode = CATTO_NULL;
    catto_AstNode* lastAstNode = CATTO_NULL;

    while (*currentToken) {
        catto_AstNode* currentAstNode = catto_parseStatement(currentToken);

        if (currentAstNode) {
            while (
                catto_eat(currentToken, CATTO_TOKEN_TYPE_STATEMENT_DELIMETER) ||
                catto_eat(currentToken, CATTO_TOKEN_TYPE_NEXT_LINE)
            ) {}
        } else {
            currentAstNode = CATTO_NEW(catto_AstNode);

            currentAstNode->type = CATTO_AST_NODE_TYPE_SYNTAX_ERROR;
            currentAstNode->nextAstNode = CATTO_NULL;
        }

        if (lastAstNode) {
            lastAstNode->nextAstNode = currentAstNode;
        }

        lastAstNode = currentAstNode;

        if (!firstAstNode) {
            firstAstNode = currentAstNode;
        }

        if (currentAstNode->type == CATTO_AST_NODE_TYPE_SYNTAX_ERROR) {
            break;
        }
    }

    return firstAstNode;
}

void catto_debugAstNodes(catto_AstNode* firstAstNode) {
    catto_AstNode* currentAstNode = firstAstNode;

    while (currentAstNode) {
        CATTO_LOG_CHAR(currentAstNode->type);

        currentAstNode = currentAstNode->nextAstNode;
    }
}