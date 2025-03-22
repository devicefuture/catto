catto_Float catto_asNumber(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return value.value.asNumber;
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_Count charactersEaten = 0;

        return catto_stringToNumber(value.value.asString, &charactersEaten);
    }

    return 0;
}

catto_TypedValue catto_asTypedNumber(catto_Float value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_NUMBER,
        .value.asNumber = value
    };
}

catto_Char* catto_asString(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return catto_numberToString(value.value.asNumber);
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        return catto_copyString(value.value.asString);
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        return catto_listToString(value.value.asList);
    }

    return catto_copyString("");
}

catto_TypedValue catto_asTypedString(catto_Char* value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_STRING,
        .value.asString = catto_copyString(value)
    };
}

catto_Bool catto_asBool(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return value.value.asNumber != 0;
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        return catto_stringLength(value.value.asString) > 0;
    }

    return CATTO_FALSE;
}

void catto_freeTypedValue(catto_TypedValue* valuePtr) {
    if (!valuePtr) {
        return;
    }

    if (valuePtr->type == CATTO_DATA_TYPE_STRING) {
        CATTO_FREE(valuePtr->value.asString);
    }

    CATTO_FREE(valuePtr);
}

catto_TypedValue catto_copyTypedValue(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        value.value.asString = catto_copyString(value.value.asString);
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        catto_referenceList(value.value.asList);
    }

    return value;
}

catto_TypedValue catto_castTypedValue(catto_TypedValue value, catto_DataType type) {
    if (type == CATTO_DATA_TYPE_NULL) {
        return catto_copyTypedValue(value);
    }

    if (type == CATTO_DATA_TYPE_NUMBER) {
        value.value.asNumber = catto_asNumber(value);
        value.type = CATTO_DATA_TYPE_NUMBER;
    }

    if (type == CATTO_DATA_TYPE_STRING) {
        value.value.asString = catto_asString(value);
        value.type = CATTO_DATA_TYPE_STRING;
    }

    return value;
}

void catto_addTypedValueToGc(catto_Context* context, catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_addPointerToGc(context, value.value.asString);
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        catto_destroyList(context, value.value.asList);
    }
}

void catto_removeTypedValueFromGc(catto_Context* context, catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_removePointerFromGc(context, value.value.asString);
    }
}