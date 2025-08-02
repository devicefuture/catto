void catto_command_dim(catto_Context* context) {
    catto_Char* identifier = catto_getIdentifierName(catto_getNextArg(context));

    if (!identifier) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
        return;
    }

    catto_List* list = catto_newList();

    while (catto_hasNextArg(context)) {
        catto_Char* fieldIdentifier = catto_getIdentifierName(catto_getNextArg(context));

        if (!fieldIdentifier) {
            context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
            return;
        }

        catto_addListField(list, fieldIdentifier);
    }

    catto_TypedValue listValue = {
        .type = CATTO_DATA_TYPE_LIST,
        .value = {.asList = list}
    };

    catto_setVariable(context, identifier, listValue);
}

void catto_command_push(catto_Context* context) {
    catto_AstNode* listArg = catto_getLastArg(context);
    catto_TypedValue listValue = catto_evalExpression(context, listArg);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_List* list = listValue.value.asList;
    catto_Count i = 0;

    while (catto_hasNextArg(context)) {
        catto_AstNode* arg = catto_getNextArg(context);

        if (arg == listArg) {
            break;
        }

        catto_TypedValue value = catto_evalExpression(context, arg);

        if (value.type == CATTO_DATA_TYPE_LIST) {
            context->errorState = CATTO_ERROR_STATE_INVALID_LIST_VALUE;
            return;
        }

        catto_pushOntoList(list, value);

        i++;
    }

    while (list->fieldCount > 0 && i % list->fieldCount > 0) {
        catto_pushOntoList(list, catto_asTypedNumber(0));
        i++;
    }
}

void catto_command_pop(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_List* list = listValue.value.asList;
    catto_Count fieldCount = list->fieldCount > 0 ? list->fieldCount : 1;
    catto_Count removalIndex = list->length - fieldCount;

    for (catto_Count i = 0; i < fieldCount; i++) {
        catto_AstNode* reassignedIdentifier = catto_getNextArg(context);
        catto_TypedValue poppedValue = catto_removeFromList(context, list, removalIndex);

        if (reassignedIdentifier) {
            catto_assignValue(context, reassignedIdentifier, poppedValue);
        }
    }
}

void catto_command_insert(catto_Context* context) {
    catto_AstNode* listArg = catto_getPenultimateArg(context);
    catto_TypedValue listValue = catto_evalExpression(context, listArg);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_List* list = listValue.value.asList;
    catto_Count fieldCount = list->fieldCount > 0 ? list->fieldCount : 1;
    catto_Int index = (catto_Int)catto_asNumber(catto_evalLastArg(context));
    catto_Count i = 0;

    while (index < 0) {
        index += listValue.value.asList->length / fieldCount;
    }

    index *= fieldCount;

    while (catto_hasNextArg(context)) {
        catto_AstNode* arg = catto_getNextArg(context);

        if (arg == listArg) {
            break;
        }

        catto_TypedValue value = catto_evalExpression(context, arg);

        if (value.type == CATTO_DATA_TYPE_LIST) {
            context->errorState = CATTO_ERROR_STATE_INVALID_LIST_VALUE;
            return;
        }

        catto_insertIntoList(list, value, index + i);

        i++;
    }

    while (list->fieldCount > 0 && i % list->fieldCount > 0) {
        catto_insertIntoList(list, catto_asTypedNumber(0), index + i);
        i++;
    }
}

void catto_command_remove(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_List* list = listValue.value.asList;
    catto_Count fieldCount = list->fieldCount > 0 ? list->fieldCount : 1;
    catto_Int index = (catto_Int)catto_asNumber(catto_evalNextArg(context));

    while (index < 0) {
        index += listValue.value.asList->length / fieldCount;
    }

    index *= fieldCount;

    for (catto_Count i = 0; i < fieldCount; i++) {
        catto_AstNode* reassignedIdentifier = catto_getNextArg(context);
        catto_TypedValue removedValue = catto_removeFromList(context, listValue.value.asList, index);

        if (reassignedIdentifier && reassignedIdentifier->type == CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
            catto_assignValue(context, reassignedIdentifier, removedValue);
        }
    }
}