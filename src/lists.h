catto_List* catto_newList() {
    catto_List* list = CATTO_NEW(catto_List);

    list->values = (catto_TypedValue*)CATTO_MALLOC(0);
    list->length = 0;
    list->fields = (catto_Char**)CATTO_MALLOC(0);
    list->fieldCount = 0;
    list->referenceCount = 0;

    return list;
}

void catto_addListField(catto_List* list, catto_Char* field) {
    list->fields = (catto_Char**)CATTO_REALLOC(list->fields, (++list->fieldCount) * sizeof(catto_Char*));
    list->fields[list->fieldCount - 1] = catto_copyString(field);
}

catto_List* catto_referenceList(catto_List* list) {
    list->referenceCount++;

    return list;
}

void catto_dereferenceList(catto_Context* context, catto_List* list) {
    if (list->referenceCount > 0) {
        list->referenceCount--;
    }

    if (list->referenceCount == 0) {
        catto_addPointerToGc(context, CATTO_DATA_TYPE_LIST, list);
    }
}

void catto_freeList(catto_Context* context, catto_List* list) {
    for (catto_Count i = 0; i < list->length; i++) {
        catto_addTypedValueToGc(context, list->values[i]);
    }

    CATTO_FREE(list->values);
    CATTO_FREE(list->fields);
    CATTO_FREE(list);
}

void catto_pushOntoList(catto_List* list, catto_TypedValue value) {
    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, (++list->length) * sizeof(catto_TypedValue));
    list->values[list->length - 1] = catto_copyTypedValue(value);
}

catto_TypedValue catto_popFromList(catto_Context* context, catto_List* list) {
    if (list->length == 0) {
        return catto_asTypedNumber(0);
    }

    catto_TypedValue value = list->values[--list->length];

    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, list->length * sizeof(catto_TypedValue));

    catto_addTypedValueToGc(context, value);

    return value;
}

void catto_insertIntoList(catto_List* list, catto_TypedValue value, catto_Count index) {
    if (index >= list->length) {
        catto_pushOntoList(list, value);

        return;
    }

    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, (++list->length) * sizeof(catto_TypedValue));

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

    list->values = (catto_TypedValue*)CATTO_REALLOC(list->values, (--list->length) * sizeof(catto_TypedValue));

    catto_addTypedValueToGc(context, value);

    return value;
}

catto_Count catto_getFlatIndex(catto_List* list, catto_Count index) {
    if (list->fieldCount == 0) {
        return index;
    }

    return index * list->fieldCount;
}

catto_Count catto_getFieldOffset(catto_List* list, catto_Char* field, catto_Bool* exists) {
    if (exists) {
        *exists = CATTO_FALSE;
    }

    if (list->fieldCount == 0 || !field) {
        return 0;
    }

    for (catto_Count i = 0; i < list->fieldCount; i++) {
        if (catto_stringsEqualCaseInsensitive(list->fields[i], field)) {
            if (exists) {
                *exists = CATTO_TRUE;
            }

            return i;
        }
    }

    return 0;
}

catto_TypedValue catto_getListItem(catto_List* list, catto_Count index) {
    if (index >= list->length) {
        return catto_asTypedNumber(0);
    }

    return list->values[index];
}

void catto_setListItem(catto_Context* context, catto_List* list, catto_Count index, catto_TypedValue value) {
    if (index >= list->length) {
        while (index > 0 && list->length < index) {
            catto_pushOntoList(list, catto_asTypedNumber(0));
        }

        catto_pushOntoList(list, value);

        while (list->fieldCount > 0 && list->length % list->fieldCount > 0) {
            catto_pushOntoList(list, catto_asTypedNumber(0));
        }

        return;
    }

    catto_addTypedValueToGc(context, list->values[index]);

    list->values[index] = value;
}

catto_Char* catto_listToString(catto_List* list) {
    if (list->length == 0) {
        return catto_copyString("");
    }

    catto_Char* string = catto_asString(list->values[0]);

    for (catto_Count i = 1; i < list->length; i++) {
        if (list->fieldCount > 0 && i % list->fieldCount == 0) {
            string = catto_appendToString(string, "\n");
        } else {
            string = catto_appendToString(string, ", ");
        }

        catto_Char* nextItemString = catto_asString(list->values[i]);

        string = catto_appendToString(string, nextItemString);

        CATTO_FREE(nextItemString);
    }

    return string;
}