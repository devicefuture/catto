void catto_command_dim(catto_Context* context) {
    catto_AstNode* identifier = catto_getNextArg(context);

    if (identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    catto_TypedValue listValue = {
        .type = CATTO_DATA_TYPE_LIST,
        .value.asList = catto_newList()
    };

    catto_setVariable(context, identifier->value.asExpressionLeaf.subjectVariable, listValue);
}

void catto_command_push(catto_Context* context) {
    catto_TypedValue value = catto_evalNextArg(context);
    catto_TypedValue listValue = catto_evalNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_INVALID_LIST_VALUE;
        return;
    }

    catto_pushOntoList(listValue.value.asList, value);
}

void catto_command_pop(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_AstNode* reassignedIdentifier = catto_getNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_TypedValue poppedValue = catto_popFromList(context, listValue.value.asList);

    if (reassignedIdentifier) {
        catto_assignValue(context, reassignedIdentifier, poppedValue);
    }
}

void catto_command_insert(catto_Context* context) {
    catto_TypedValue value = catto_evalNextArg(context);
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_Int index = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_INVALID_LIST_VALUE;
        return;
    }

    while (index < 0) {
        index += listValue.value.asList->length;
    }

    catto_insertIntoList(listValue.value.asList, value, index);
}

void catto_command_remove(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_Int index = (catto_Int)catto_asNumber(catto_evalNextArg(context));
    catto_AstNode* reassignedIdentifier = catto_getNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    while (index < 0) {
        index += listValue.value.asList->length;
    }

    catto_TypedValue removedValue = catto_removeFromList(context, listValue.value.asList, index);

    if (reassignedIdentifier && reassignedIdentifier->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        catto_assignValue(context, reassignedIdentifier, removedValue);
    }
}