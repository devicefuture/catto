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

    return catto_copyString("");
}

catto_TypedValue catto_asTypedString(catto_Char* value) {
    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_STRING,
        .value.asString = catto_copyString(value)
    };
}