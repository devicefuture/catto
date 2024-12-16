catto_Char* catto_asString(catto_TypedValue value) {
    if (value.type == CATTO_DATA_TYPE_NUMBER) {
        return catto_numberToString(value.value.asNumber);
    }

    if (value.type == CATTO_DATA_TYPE_STRING) {
        return catto_copyString(value.value.asString);
    }

    return catto_copyString("");
}