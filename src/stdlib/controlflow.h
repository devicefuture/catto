void catto_command_goto(catto_Context* context) {
    catto_Int lineNumber = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (lineNumber < 0) {
        lineNumber = 0;
    }

    catto_goto(context, lineNumber);
}

void catto_command_gosub(catto_Context* context) {
    catto_Int lineNumber = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (lineNumber < 0) {
        lineNumber = 0;
    }

    catto_pushOntoStatementStack(context, context->nextParsedStatement);

    catto_goto(context, lineNumber);
}

void catto_command_return(catto_Context* context) {
    catto_AstNode* statement = catto_popFromStatementStack(context);

    if (!statement) {
        context->errorState = CATTO_ERROR_STATE_NO_RETURN;
        return;
    }

    context->nextParsedStatement = statement;
}

void catto_command_if(catto_Context* context) {
    catto_Bool isTrue = catto_asBool(catto_evalNextArg(context));

    catto_AstNode* elseStatement = catto_findClosingMark(context->currentParsedStatement, "else", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end", CATTO_MARK_SEARCH_ALL);

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

    catto_AstNode* endStatement = catto_findClosingMark(context->currentParsedStatement, "end", CATTO_MARK_SEARCH_ALL);

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

    catto_AstNode* elseStatement = catto_findClosingMark(context->currentParsedStatement, "else", CATTO_MARK_SEARCH_ALL);

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
    if (!catto_findClosingMark(context->currentParsedStatement, "next", CATTO_MARK_SEARCH_ALL)) {
        context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
        return;
    }

    catto_AstNode* identifier = catto_getNextArg(context);
    catto_TypedValue start = catto_evalNextArg(context);

    if (identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_assignValue(context, identifier, start);
}

void catto_command_next(catto_Context* context) {
    catto_AstNode* forStatement = catto_findOpeningMark(context->currentParsedStatement, "for", CATTO_MARK_SEARCH_ALL);

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

    catto_assignValue(context, identifier, catto_asTypedNumber(currentValue + catto_asNumber(step)));

    context->nextParsedStatement = forStatement->nextAstNode;
}

void catto_command_repeat(catto_Context* context) {
    catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_ALL);

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
            catto_AstNode* repeatStatement = catto_findOpeningMark(context->currentParsedStatement, "repeat", CATTO_MARK_SEARCH_ALL);

            if (repeatStatement) {
                context->nextParsedStatement = repeatStatement;
            } else {
                context->errorState = CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK;
            }
        }

        return;
    }

    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_ALL);

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
    catto_AstNode* repeatStatement = catto_findOpeningMark(context->currentParsedStatement, "repeat", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* whileStatement = catto_findOpeningMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_ALL);
    catto_AstNode* untilStatement = catto_findOpeningMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_ALL);

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

void catto_command_break(catto_Context* context) {
    catto_AstNode* openingStatement = catto_findOpeningMark(context->currentParsedStatement, CATTO_NULL, CATTO_MARK_SEARCH_LOOP_ONLY);

    if (!openingStatement) {
        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    if (catto_isCommand(openingStatement, "repeat")) {
        catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

        if (whileStatement) {
            context->nextParsedStatement = whileStatement->nextAstNode;
            return;
        }

        if (untilStatement) {
            context->nextParsedStatement = untilStatement->nextAstNode;
            return;
        }

        if (loopStatement) {
            context->nextParsedStatement = loopStatement->nextAstNode;
            return;
        }

        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    catto_AstNode* nextStatement = catto_findClosingMark(context->currentParsedStatement, "next", CATTO_MARK_SEARCH_LOOP_ONLY);
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

    if (nextStatement) {
        context->nextParsedStatement = nextStatement->nextAstNode;
        return;
    }

    if (loopStatement) {
        context->nextParsedStatement = loopStatement->nextAstNode;
        return;
    }

    context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
}

void catto_command_continue(catto_Context* context) {
    catto_AstNode* openingStatement = catto_findOpeningMark(context->currentParsedStatement, CATTO_NULL, CATTO_MARK_SEARCH_LOOP_ONLY);

    if (!openingStatement) {
        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    if (catto_isCommand(openingStatement, "repeat")) {
        catto_AstNode* whileStatement = catto_findClosingMark(context->currentParsedStatement, "while", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* untilStatement = catto_findClosingMark(context->currentParsedStatement, "until", CATTO_MARK_SEARCH_LOOP_ONLY);
        catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

        if (whileStatement) {
            context->nextParsedStatement = whileStatement;
            return;
        }

        if (untilStatement) {
            context->nextParsedStatement = untilStatement;
            return;
        }

        if (loopStatement) {
            context->nextParsedStatement = loopStatement;
            return;
        }

        context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
        return;
    }

    catto_AstNode* nextStatement = catto_findClosingMark(context->currentParsedStatement, "next", CATTO_MARK_SEARCH_LOOP_ONLY);
    catto_AstNode* loopStatement = catto_findClosingMark(context->currentParsedStatement, "loop", CATTO_MARK_SEARCH_LOOP_ONLY);

    if (nextStatement) {
        context->nextParsedStatement = nextStatement;
        return;
    }

    if (loopStatement) {
        context->nextParsedStatement = loopStatement;
        return;
    }

    context->errorState = CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP;
}

void catto_command_stop(catto_Context* context) {
    context->nextParsedStatement = CATTO_NULL;
}