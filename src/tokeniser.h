#ifndef CATTO_TOKENISER_H_
#define CATTO_TOKENISER_H_

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

catto_Bool catto_onWordBoundary(const catto_Char* code, catto_Count index) {
    return !(
        (code[index] >= 'a' && code[index] <= 'z') ||
        (code[index] >= 'A' && code[index] <= 'Z') ||
        (code[index] >= '0' && code[index] <= '9') ||
        code[index] == '_'
    );
}

catto_Token* catto_matchLineNumber(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
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

catto_Token* catto_matchComment(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;

    if (!(code[index] == '#' || catto_stringStartsWithCaseInsensitive(code + index, "rem "))) {
        return CATTO_NULL;
    }

    while (code[index] != '\n' && code[index] != '\0') {
        index++;
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_COMMENT, currentTokenPtr);

    *indexPtr = index;

    return token;
}

catto_Token* catto_matchChar(catto_Char matchChar, catto_TokenType type, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    if (code[*indexPtr] != matchChar) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(type, currentTokenPtr);

    (*indexPtr)++;

    return token;
}

catto_Token* catto_matchCommand(catto_Context* context, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_CommandHandler* currentCommandHandler = context->firstCommandHandler;

    while (currentCommandHandler) {
        if (catto_stringStartsWithCaseInsensitive(code + index, currentCommandHandler->name)) {
            catto_Count newIndex = index + catto_stringLength(currentCommandHandler->name);

            if (code[newIndex] == '$' || code[newIndex] == '%') {
                goto skipCommandHandler;
            }

            if (!context->scrawlMode && !catto_onWordBoundary(code, newIndex)) {
                goto skipCommandHandler;
            }

            if (catto_stringsEqualCaseInsensitive(currentCommandHandler->name, "scrawl")) {
                context->scrawlMode = CATTO_TRUE;
            }

            if (catto_stringsEqualCaseInsensitive(currentCommandHandler->name, "noscrawl")) {
                context->scrawlMode = CATTO_FALSE;
            }

            catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_COMMAND, currentTokenPtr);

            token->value.asCommandHandler = currentCommandHandler;

            *indexPtr = newIndex;

            return token;
        }

        skipCommandHandler:

        currentCommandHandler = currentCommandHandler->nextCommandHandler;
    }

    return CATTO_NULL;
}

catto_Token* catto_matchStrings(catto_Context* context, const catto_Char** matchStrings, catto_TokenType type, catto_Bool caseInsensitive, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_Count i = 0;

    while (matchStrings[i]) {
        const catto_Char* currentString = matchStrings[i];

        if (_catto_stringStartsWith(code + index, currentString, caseInsensitive)) {
            catto_Count newIndex = index + catto_stringLength(currentString);

            if (!context->scrawlMode && !catto_onWordBoundary(code, newIndex)) {
                goto skipMatch;
            }

            catto_Token* token = catto_addToken(type, currentTokenPtr);

            token->value.asConstString = currentString;

            *indexPtr = index + catto_stringLength(currentString);

            return token;
        }

        skipMatch:

        i++;
    }

    return CATTO_NULL;
}

catto_Token* catto_matchNumber(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count charactersEaten = 0;
    catto_Float number = catto_unsignedStringToNumber(code + *indexPtr, &charactersEaten);

    if (number == CATTO_NAN || charactersEaten == 0) {
        return CATTO_NULL;
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_NUMBER, currentTokenPtr);

    token->value.asNumber = number;

    *indexPtr += charactersEaten;

    return token;
}

catto_Token* catto_matchStringLiteral(const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;

    catto_Char stringOpener = code[index++];
    catto_Char* currentString = (catto_Char*)CATTO_MALLOC(8);
    catto_Count currentStringIndex = 0;

    currentString[currentStringIndex] = '\0';

    if (stringOpener != '"' && stringOpener != '\'' && stringOpener != '`') {
        CATTO_FREE(currentString);

        return CATTO_NULL;
    }

    while (CATTO_TRUE) {
        catto_Char currentChar = code[index++];

        if (currentChar == stringOpener) {
            break;
        }

        if (currentChar == '\0' || currentChar == '\n') {
            CATTO_FREE(currentString);

            return CATTO_NULL;
        }

        if (currentChar == '\\') {
            switch (code[index]) {
                case '\0':
                case '\n':
                    CATTO_FREE(currentString);

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

        if ((currentStringIndex + 1) % 8 == 0) {
            currentString = (catto_Char*)CATTO_REALLOC(currentString, currentStringIndex + 9);
        }
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_STRING, currentTokenPtr);

    token->value.asString = currentString;

    *indexPtr = index;

    return token;
}

catto_Token* catto_matchIdentifier(catto_Context* context, const catto_Char* code, catto_Count* indexPtr, catto_Token** currentTokenPtr) {
    catto_Count index = *indexPtr;
    catto_Char* currentString = (catto_Char*)CATTO_MALLOC(8);
    catto_Count currentStringIndex = 0;
    catto_Char currentChar = code[index++];

    if (!(
        (currentChar >= 'a' && currentChar <= 'z') ||
        (currentChar >= 'A' && currentChar <= 'Z') ||
        currentChar == '_'
    )) {
        CATTO_FREE(currentString);

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
            (!context->scrawlMode && currentChar >= '0' && currentChar <= '9') ||
            currentChar == '_'
        )) {
            index--;

            break;
        }

        currentString[currentStringIndex++] = currentChar;
        currentString[currentStringIndex] = '\0';

        if ((currentStringIndex + 1) % 8 == 0) {
            currentString = (catto_Char*)CATTO_REALLOC(currentString, currentStringIndex + 9);
        }
    }

    catto_Token* token = catto_addToken(CATTO_TOKEN_TYPE_IDENTIFIER, currentTokenPtr);

    token->value.asString = currentString;

    *indexPtr = index;

    return token;
}

catto_Token* catto_tokenise(catto_Context* context, const catto_Char* code) {
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

        if (catto_matchComment(code, &index, &currentToken)) {
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

        if (catto_matchStrings(context, catto_operators, CATTO_TOKEN_TYPE_OPERATOR, CATTO_TRUE, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchCommand(context, code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchStringLiteral(code, &index, &currentToken)) {
            continue;
        }

        if (catto_matchIdentifier(context, code, &index, &currentToken)) {
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

void catto_freeTokens(catto_Token* firstToken) {
    catto_Token* currentToken = firstToken;

    while (currentToken) {
        catto_Token* lastToken = currentToken;

        switch (currentToken->type) {
            case CATTO_TOKEN_TYPE_STRING:
            case CATTO_TOKEN_TYPE_IDENTIFIER:
                CATTO_FREE(currentToken->value.asString);
                break;

            default:
                break;
        }

        currentToken = currentToken->nextToken;

        CATTO_FREE(lastToken);
    }
}

void catto_debugTokens(catto_Token* firstToken) {
    catto_Token* currentToken = firstToken;

    while (currentToken) {
        CATTO_LOG_CHAR(currentToken->type);

        currentToken = currentToken->nextToken;
    }
}

#endif