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
        .value = {.asNumber = value}
    };
}

CATTO_THROWS(CATTO_NULL) catto_Char* catto_asString(catto_TypedValue value) {
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

CATTO_THROWS(CATTO_TYPED_ZERO) catto_TypedValue catto_asTypedString(const catto_Char* value) {
    catto_Char* string = catto_copyString(value); CATTO_MUST_R(string, CATTO_TYPED_ZERO);

    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_STRING,
        .value = {.asString = string}
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

CATTO_THROWS(CATTO_TYPED_ZERO) catto_TypedValue catto_copyTypedValue(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_Char* string = catto_copyString(value.value.asString); CATTO_MUST_R(string, CATTO_TYPED_ZERO);

        value.value.asString = string;
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        catto_referenceList(value.value.asList);
    }

    return value;
}

CATTO_THROWS(CATTO_TYPED_ZERO) catto_TypedValue catto_castTypedValue(catto_TypedValue value, catto_DataType type) {
    if (type == CATTO_DATA_TYPE_NULL) {
        return catto_copyTypedValue(value);
    }

    if (type == CATTO_DATA_TYPE_NUMBER) {
        value.value.asNumber = catto_asNumber(value);
        value.type = CATTO_DATA_TYPE_NUMBER;
    }

    if (type == CATTO_DATA_TYPE_STRING) {
        catto_Char* string = catto_asString(value); CATTO_MUST_R(string, CATTO_TYPED_ZERO);

        value.value.asString = string;
        value.type = CATTO_DATA_TYPE_STRING;
    }

    return value;
}

CATTO_THROWS(CATTO_FALSE) catto_Bool catto_addTypedValueToGc(catto_Context* context, catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        CATTO_MUST_F(catto_addPointerToGc(context, CATTO_DATA_TYPE_STRING, value.value.asString));
    }

    if (value.type == CATTO_DATA_TYPE_LIST) {
        CATTO_MUST_F(catto_dereferenceList(context, value.value.asList));
    }

    return CATTO_TRUE;
}

void catto_removeTypedValueFromGc(catto_Context* context, catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_STRING) {
        catto_removePointerFromGc(context, value.value.asString);
    }
}