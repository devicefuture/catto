catto_List* catto_newList() {
    catto_List* list = CATTO_NEW(catto_List);

    list->values = CATTO_MALLOC(0);
    list->length = 0;
    list->referenceCount = 0;

    return list;
}

catto_List* catto_referenceList(catto_List* list) {
    list->referenceCount++;

    return list;
}

void catto_destroyList(catto_Context* context, catto_List* list) {
    if (list->referenceCount > 0) {
        list->referenceCount--;
    }

    if (list->referenceCount == 0) {
        for (catto_Count i = 0; i < list->length; i++) {
            catto_addTypedValueToGc(context, list->values[i]);
        }
    }
}

void catto_pushOntoList(catto_List* list, catto_TypedValue value) {
    list->values = CATTO_REALLOC(list->values, (++list->length) * sizeof(catto_TypedValue));
    list->values[list->length - 1] = catto_copyTypedValue(value);
}

catto_TypedValue catto_popFromList(catto_Context* context, catto_List* list) {
    if (list->length == 0) {
        return catto_asTypedNumber(0);
    }

    catto_TypedValue value = list->values[--list->length];

    list->values = CATTO_REALLOC(list->values, list->length * sizeof(catto_TypedValue));

    catto_addTypedValueToGc(context, value);

    return value;
}

void catto_insertIntoList(catto_List* list, catto_TypedValue value, catto_Count index) {
    if (index >= list->length) {
        catto_pushOntoList(list, value);
        return;
    }

    list->values = CATTO_REALLOC(list->values, (++list->length) * sizeof(catto_TypedValue));

    for (catto_Count i = list->length - 1; i > index; i--) {
        list->values[i] = list->values[i - 1];
    }

    list->values[index] = catto_copyTypedValue(value);
}

catto_TypedValue catto_removeFromList(catto_Context* context, catto_List* list, catto_Count index) {
    if (index >= list->length - 1) {
        return catto_popFromList(context, list);
    }

    catto_TypedValue value = list->values[index];

    for (catto_Count i = index; i < list->length - 1; i++) {
        list->values[i] = list->values[i + 1];
    }

    list->values = CATTO_REALLOC(list->values, (--list->length) * sizeof(catto_TypedValue));

    catto_addTypedValueToGc(context, value);

    return value;
}

catto_Char* catto_listToString(catto_List* list) {
    if (list->length == 0) {
        return catto_copyString("");
    }

    catto_Char* string = catto_asString(list->values[0]);

    for (catto_Count i = 1; i < list->length; i++) {
        string = catto_appendToString(string, ", ");

        catto_Char* nextItemString = catto_asString(list->values[i]);

        string = catto_appendToString(string, nextItemString);

        CATTO_FREE(nextItemString);
    }

    return string;
}