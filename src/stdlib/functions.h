catto_TypedValue catto_function_round(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));
    catto_Int roundedValue = (catto_Int)(value < 0 ? value - 0.5 : value + 0.5);

    return catto_asTypedNumber((catto_Float)roundedValue);
}

catto_TypedValue catto_function_floor(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));
    catto_Int flooredValue = (catto_Int)(value < 0 ? value - 1 : value);

    return catto_asTypedNumber((catto_Float)flooredValue);
}

catto_TypedValue catto_function_ceil(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));
    catto_Int flooredValue = (catto_Int)(value < 0 ? value - 1 : value);

    return catto_asTypedNumber((catto_Float)(value == flooredValue ? flooredValue : flooredValue + 1));
}

catto_TypedValue catto_function_abs(catto_Context* context, catto_DataType returnType) {
    catto_Float value = catto_asNumber(catto_evalNextArg(context));

    if (value < 0) {
        value *= -1;
    }

    return catto_asTypedNumber(value);
}

catto_TypedValue catto_function_min(catto_Context* context, catto_DataType returnType) {
    catto_Float a = catto_asNumber(catto_evalNextArg(context));
    catto_Float b = catto_asNumber(catto_evalNextArg(context));

    return catto_asTypedNumber(b < a ? b : a);
}

catto_TypedValue catto_function_max(catto_Context* context, catto_DataType returnType) {
    catto_Float a = catto_asNumber(catto_evalNextArg(context));
    catto_Float b = catto_asNumber(catto_evalNextArg(context));

    return catto_asTypedNumber(b > a ? b : a);
}

catto_TypedValue catto_function_asc(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));

    catto_TypedValue returnValue = catto_asTypedNumber(value[0]);

    CATTO_FREE(value);

    return returnValue;
}

catto_TypedValue catto_function_chr(catto_Context* context, catto_DataType returnType) {
    catto_Int codepoint = catto_asNumber(catto_evalNextArg(context));
    catto_Char* string = catto_copyString("");

    if (codepoint > 0) {
        string = catto_appendCharToString(string, codepoint);
    }

    catto_TypedValue returnValue = catto_asTypedString(string);

    CATTO_FREE(string);

    return returnValue;
}

catto_TypedValue catto_function_lower(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Char* currentChar = value;

    while (*currentChar != '\0') {
        if (*currentChar >= 'A' && *currentChar <= 'Z') {
            *currentChar += 'a' - 'A';
        }

        currentChar++;
    }

    catto_TypedValue returnValue = catto_asTypedString(value);

    CATTO_FREE(value);

    return returnValue;
}

catto_TypedValue catto_function_upper(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Char* currentChar = value;

    while (*currentChar != '\0') {
        if (*currentChar >= 'a' && *currentChar <= 'z') {
            *currentChar -= 'a' - 'A';
        }

        currentChar++;
    }

    catto_TypedValue returnValue = catto_asTypedString(value);

    CATTO_FREE(value);

    return returnValue;
}