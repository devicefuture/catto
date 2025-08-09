#ifndef CATTOX_NO_CSV

catto_Char* _cattox_csv_formatValue(catto_TypedValue value) {
    switch (value.type) {
        case CATTO_DATA_TYPE_NUMBER:
            return catto_asString(value);

        case CATTO_DATA_TYPE_STRING:
        {
            catto_Char* string = catto_asString(value);
            catto_Char* returnValue = catto_copyString("\"");
            catto_Count i = 0;

            while (string[i]) {
                if (string[i] == '"') {
                    // Escape quotes by duplicating them
                    returnValue = catto_appendCharToString(returnValue, '"');
                }

                returnValue = catto_appendCharToString(returnValue, string[i]);

                i++;
            }

            CATTO_FREE(string);

            returnValue = catto_appendCharToString(returnValue, '"');

            return returnValue;
        }

        default:
            return catto_copyString("");
    }
}

catto_Char* _cattox_csv_fromlist(catto_List* list) {
    catto_Char* csv = catto_copyString("");

    for (catto_Count i = 0; i < list->fieldCount; i++) {
        csv = catto_appendToString(csv, list->fields[i]);
        csv = catto_appendCharToString(csv, i == list->fieldCount - 1 ? '\n' : ',');
    }

    for (catto_Count i = 0; i < list->length; i++) {
        catto_Char* formattedString = _cattox_csv_formatValue(list->values[i]);

        csv = catto_appendToString(csv, formattedString);

        if (i < list->length - 1) {
            csv = catto_appendCharToString(csv, i % list->fieldCount == list->fieldCount - 1 ? '\n' : ',');
        }

        CATTO_FREE(formattedString);
    }

    return csv;
}

void cattox_csv_fromlist(catto_Context* context) {
    catto_TypedValue listValue = catto_evalNextArg(context);
    catto_Char* identifier = catto_getIdentifierName(catto_getNextArg(context));

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    if (!identifier) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
        return;
    }

    catto_Char* csvString = _cattox_csv_fromlist(listValue.value.asList);
    catto_TypedValue csvValue = catto_asTypedString(csvString);

    catto_setVariable(context, identifier, csvValue);

    catto_addTypedValueToGc(context, csvValue);

    CATTO_FREE(csvString);
}

catto_Bool _cattox_csv_parseNextFieldName(catto_Char* csv, catto_Count* indexPtr, catto_Char** fieldNamePtr) {
    catto_Char* fieldName = catto_copyString("");
    catto_Bool hadFirstCharacter = CATTO_FALSE;

    while (csv[*indexPtr] != '\0') {
        catto_Char currentChar = csv[*indexPtr];

        if (
            (currentChar >= 'a' && currentChar <= 'z') ||
            (currentChar >= 'A' && currentChar <= 'Z') ||
            (hadFirstCharacter && currentChar >= '0' && currentChar <= '9') ||
            currentChar == '_' ||
            (hadFirstCharacter && (currentChar == '$' || currentChar == '%'))
        ) {
            fieldName = catto_appendCharToString(fieldName, currentChar);
            hadFirstCharacter = CATTO_TRUE;

            (*indexPtr)++;

            if (currentChar == '$' || currentChar == '%') {
                break;
            }
        } else {
            break;
        }
    }

    printf("FN: %s\n", fieldName);

    if (
        catto_stringLength(fieldName) == 0 ||
        (csv[*indexPtr] != ',' && csv[*indexPtr] != '\n' && csv[*indexPtr] != '\0')
    ) {
        CATTO_FREE(fieldName);

        if (fieldNamePtr) {
            *fieldNamePtr = CATTO_NULL;
        }

        return CATTO_FALSE;
    }

    if (fieldNamePtr) {
        *fieldNamePtr = fieldName;
    }

    if (csv[*indexPtr]) {
        (*indexPtr)++;
    }

    return csv[(*indexPtr) - 1] == ',';
}

catto_Bool _cattox_csv_parseNextValue(catto_Char* csv, catto_Count* indexPtr, catto_Char** valuePtr, catto_DataType* returnDataType) {
    catto_Char* value = catto_copyString("");
    catto_Bool inQuotedString = CATTO_FALSE;
    catto_Bool justHadQuote = CATTO_FALSE;

    if (returnDataType) {
        *returnDataType = CATTO_DATA_TYPE_NUMBER;
    }

    while (csv[*indexPtr] != '\0') {
        catto_Char currentChar = csv[*indexPtr];

        if (!inQuotedString && (currentChar == ',' || currentChar == '\n')) {
            break;
        }

        (*indexPtr)++;

        if (currentChar == '"') {
            inQuotedString = !inQuotedString;

            if (returnDataType) {
                *returnDataType = CATTO_DATA_TYPE_STRING;
            }

            if (!justHadQuote) {
                justHadQuote = CATTO_TRUE;
                continue;
            }
        }

        value = catto_appendCharToString(value, currentChar);
        justHadQuote = CATTO_FALSE;
    }

    *valuePtr = value;

    if (csv[*indexPtr]) {
        (*indexPtr)++;
    }

    return csv[(*indexPtr) - 1] == ',';
}

catto_List* _cattox_csv_tolist(catto_Context* context, catto_Char* csv) {
    catto_List* list = catto_newList();
    catto_Count index = 0;
    catto_Char* currentFieldName = CATTO_NULL;
    catto_Char* currentValue = CATTO_NULL;
    catto_DataType currentDataType = CATTO_DATA_TYPE_NULL;
    catto_Count recordIndex = 0;
    catto_Count fieldIndex = 0;

    while (csv[index] != '\0') {
        catto_Bool hasNextField = _cattox_csv_parseNextFieldName(csv, &index, &currentFieldName);

        if (!currentFieldName) {
            context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
            return CATTO_NULL;
        }

        catto_addListField(list, currentFieldName);

        CATTO_FREE(currentFieldName);

        if (!hasNextField) {
            break;
        }
    }

    while (csv[index] != '\0') {
        catto_Bool hasNextValueOnRecord = _cattox_csv_parseNextValue(csv, &index, &currentValue, &currentDataType);

        if (fieldIndex < list->fieldCount) {
            catto_Count flatIndex = catto_getFlatIndex(list, recordIndex) + fieldIndex;
            catto_TypedValue value = catto_asTypedNumber(0);

            if (currentDataType == CATTO_DATA_TYPE_NUMBER) {
                catto_Count charactersEaten = 0;
                catto_Float number = catto_stringToNumber(currentValue, &charactersEaten);

                if (charactersEaten == catto_stringLength(currentValue)) {
                    value = catto_asTypedNumber(number);
                } else {
                    // Value was just an unquoted string; fall back to being string
                    currentDataType = CATTO_DATA_TYPE_STRING;
                }
            }

            if (currentDataType == CATTO_DATA_TYPE_STRING) {
                value = catto_asTypedString(currentValue);
            }

            catto_setListItem(context, list, flatIndex, value);

            catto_addTypedValueToGc(context, value);
        }

        if (hasNextValueOnRecord) {
            fieldIndex++;
        } else {
            fieldIndex = 0;
            recordIndex++;
        }

        CATTO_FREE(currentValue);
    }

    return list;
}

void cattox_csv_tolist(catto_Context* context) {
    catto_Char* csvString = catto_asString(catto_evalNextArg(context));
    catto_Char* identifier = catto_getIdentifierName(catto_getNextArg(context));

    if (!identifier) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
        CATTO_FREE(csvString);
        return;
    }

    catto_List* list = _cattox_csv_tolist(context, csvString);

    if (!list) {
        CATTO_FREE(csvString);
        return;
    }

    catto_TypedValue listValue = {
        .type = CATTO_DATA_TYPE_LIST,
        .value = {.asList = list}
    };

    catto_setVariable(context, identifier, listValue);

    CATTO_FREE(csvString);
}

void _cattox_csv_intolist(catto_Context* context, catto_Char* csv, catto_List* list) {
    catto_List* newList = _cattox_csv_tolist(context, csv);

    if (!newList) {
        return;
    }

    if (newList->fieldCount == 0) {
        catto_freeList(context, newList);
        return;
    }

    for (catto_Count newFieldIndex = 0; newFieldIndex < newList->fieldCount; newFieldIndex++) {
        catto_Bool exists = CATTO_FALSE;
        catto_Count fieldIndex = catto_getFieldOffset(list, newList->fields[newFieldIndex], &exists);

        if (!exists) {
            continue;
        }

        for (catto_Count i = 0; i < newList->length / newList->fieldCount; i++) {
            catto_Count index = (i * list->fieldCount) + fieldIndex;
            catto_Count newIndex = (i * newList->fieldCount) + newFieldIndex;

            catto_setListItem(context, list, index, newList->values[newIndex]);
        }
    }

    catto_freeList(context, newList);
}

void cattox_csv_intolist(catto_Context* context) {
    catto_Char* csvString = catto_asString(catto_evalNextArg(context));
    catto_AstNode* listArg = catto_getLastArg(context);
    catto_TypedValue listValue = catto_evalExpression(context, listArg);

    if (listValue.type != CATTO_DATA_TYPE_LIST) {
        context->errorState = CATTO_ERROR_STATE_NOT_A_LIST;
        return;
    }

    catto_List* list = listValue.value.asList;

    _cattox_csv_intolist(context, csvString, list);

    CATTO_FREE(csvString);
}

void cattox_csv_init(catto_Context* context) {
    cattox_Extension* extension = cattox_newExtension(context, "csv");

    cattox_addExtensionCommand(extension, "fromlist", cattox_csv_fromlist);
    cattox_addExtensionCommand(extension, "tolist", cattox_csv_tolist);
    cattox_addExtensionCommand(extension, "intolist", cattox_csv_intolist);
}

#endif