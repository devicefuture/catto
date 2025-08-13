#ifdef CATTO_USE_64_BIT
    #define CATTO_RANDOM_MASK 0xFFFFFFFFFFFFF
#else
    #define CATTO_RANDOM_MASK 0xFFFFFF
#endif

#define CATTO_TRIG_MODE_COMMAND(name, mode) void name(catto_Context* context) { \
        context->trigMode = mode; \
    }

#define CATTO_UNARY_NUMERIC_FUNCTION(name, callName) catto_TypedValue name(catto_Context* context, catto_DataType returnType) { \
        catto_Float value = catto_asNumber(catto_evalNextArg(context)); \
        \
        return catto_asTypedNumber(callName(value)); \
    }

#define CATTO_TRIG_FUNCTION(name, callName) catto_TypedValue name(catto_Context* context, catto_DataType returnType) { \
        catto_Float value = catto_asNumber(catto_evalNextArg(context)); \
        catto_Float convertedValue = catto_toRadians(value, context->trigMode); \
        \
        return catto_asTypedNumber(callName(convertedValue)); \
    }

#define CATTO_TRIG_ARC_FUNCTION(name, callName) catto_TypedValue name(catto_Context* context, catto_DataType returnType) { \
        catto_Float value = catto_asNumber(catto_evalNextArg(context)); \
        \
        return catto_asTypedNumber(catto_fromRadians(callName(value), context->trigMode)); \
    }

CATTO_TRIG_MODE_COMMAND(catto_command_deg, CATTO_TRIG_MODE_DEGREES);
CATTO_TRIG_MODE_COMMAND(catto_command_rad, CATTO_TRIG_MODE_RADIANS);
CATTO_TRIG_MODE_COMMAND(catto_command_gon, CATTO_TRIG_MODE_GRADIANS);
CATTO_TRIG_MODE_COMMAND(catto_command_turn, CATTO_TRIG_MODE_TURNS);

CATTO_TRIG_FUNCTION(catto_function_sin, catto_sin);
CATTO_TRIG_FUNCTION(catto_function_cos, catto_cos);
CATTO_TRIG_FUNCTION(catto_function_tan, catto_tan);

CATTO_TRIG_ARC_FUNCTION(catto_function_asin, catto_asin);
CATTO_TRIG_ARC_FUNCTION(catto_function_acos, catto_acos);
CATTO_TRIG_ARC_FUNCTION(catto_function_atan, catto_atan);

CATTO_UNARY_NUMERIC_FUNCTION(catto_function_log, catto_log);
CATTO_UNARY_NUMERIC_FUNCTION(catto_function_ln, catto_ln);
CATTO_UNARY_NUMERIC_FUNCTION(catto_function_sqrt, catto_sqrt);

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

catto_TypedValue catto_function_base(catto_Context* context, catto_Count base, catto_DataType returnType) {
    if (returnType == CATTO_DATA_TYPE_STRING) {
        catto_Float number = catto_asNumber(catto_evalNextArg(context));
        catto_Char* string = catto_numberToBaseString(number, base);
        catto_TypedValue returnValue = catto_asTypedString(string);

        CATTO_FREE(string);

        return returnValue;
    }

    catto_Char* string = catto_asString(catto_evalNextArg(context));

    catto_Count charactersEaten;
    catto_TypedValue returnValue = catto_asTypedNumber(catto_stringToBaseNumber(string, base, &charactersEaten));

    CATTO_FREE(string);

    return returnValue;
}

catto_TypedValue catto_function_bin(catto_Context* context, catto_DataType returnType) {
    return catto_function_base(context, 2, returnType);
}

catto_TypedValue catto_function_oct(catto_Context* context, catto_DataType returnType) {
    return catto_function_base(context, 8, returnType);
}

catto_TypedValue catto_function_hex(catto_Context* context, catto_DataType returnType) {
    return catto_function_base(context, 16, returnType);
}

catto_TypedValue catto_function_len(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue value = catto_evalNextArg(context);

    if (value.type == CATTO_DATA_TYPE_LIST) {
        catto_List* list = value.value.asList;
        catto_Count fieldCount = list->fieldCount > 0 ? list->fieldCount : 1;

        return catto_asTypedNumber(list->length / fieldCount);
    }

    catto_Char* valueString = catto_asString(value);
    catto_TypedValue length = catto_asTypedNumber(catto_stringLength(valueString));

    CATTO_FREE(valueString);

    return length;
}

catto_TypedValue catto_function_last(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue value = catto_evalNextArg(context);

    if (value.type != CATTO_DATA_TYPE_LIST) {
        return catto_asTypedNumber(0);
    }

    catto_List* list = value.value.asList;

    if (list->length == 0) {
        return catto_asTypedNumber(0);
    }

    catto_TypedValue returnValue = catto_copyTypedValue(list->values[list->length - 1]);

    return returnValue;
}

catto_TypedValue catto_function_split(catto_Context* context, catto_DataType returnType) {
    catto_Char* string = catto_asString(catto_evalNextArg(context));
    catto_Char* delimeter = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString("");
    catto_Count delimeterLength = catto_stringLength(delimeter);
    catto_List* list = catto_newList();
    catto_Char* currentString = catto_copyString("");

    catto_Count i = 0;

    while (CATTO_TRUE) {
        if (!string[i]) {
            goto splitHere;
        }

        if (delimeterLength > 0 && catto_stringStartsWith(string + i, delimeter)) {
            goto splitHere;
        }

        currentString = catto_appendCharToString(currentString, string[i]);

        i++;

        if (delimeterLength > 0) {
            continue;
        }

        splitHere: ;

        catto_TypedValue typedString = catto_asTypedString(currentString);

        catto_pushOntoList(list, typedString);
        catto_addTypedValueToGc(context, typedString);

        CATTO_FREE(currentString);

        currentString = catto_copyString("");

        if (!string[i]) {
            break;
        }

        i += delimeterLength;

        continue;
    }

    CATTO_FREE(string);
    CATTO_FREE(delimeter);
    CATTO_FREE(currentString);

    return (catto_TypedValue) {
        .type = CATTO_DATA_TYPE_LIST,
        .value = {.asList = list}
    };
}

catto_TypedValue catto_function_join(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue value = catto_evalNextArg(context);

    if (value.type != CATTO_DATA_TYPE_LIST) {
        return catto_asTypedString("");
    }

    catto_List* list = value.value.asList;
    catto_Char* resultString = catto_copyString("");
    catto_Char* delimeter = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString("");
    catto_Count delimeterLength = catto_stringLength(delimeter);

    for (catto_Count i = 0; i < list->length; i++) {
        catto_Char* partString = catto_asString(list->values[i]);

        if (i > 0 && delimeterLength > 0) {
            resultString = catto_appendToString(resultString, delimeter);
        }

        resultString = catto_appendToString(resultString, partString);

        CATTO_FREE(partString);
    }

    catto_TypedValue returnValue = catto_asTypedString(resultString);

    CATTO_FREE(resultString);
    CATTO_FREE(delimeter);

    return returnValue;
}

catto_TypedValue catto_function_find(catto_Context* context, catto_DataType returnType) {
    catto_TypedValue sequence = catto_evalNextArg(context);
    catto_AstNode* searchFieldArg = catto_getNextArg(context);
    catto_AstNode* searchValueArg = catto_getNextArg(context);

    if (!searchValueArg) {
        searchValueArg = searchFieldArg;
        searchFieldArg = CATTO_NULL;
    }

    catto_TypedValue searchValue = catto_evalExpression(context, searchValueArg);

    if (sequence.type == CATTO_DATA_TYPE_LIST) {
        catto_List* list = sequence.value.asList;
        catto_Char* field = searchFieldArg ? catto_asString(catto_evalExpression(context, searchFieldArg)) : CATTO_NULL;
        catto_Count fieldCount = list->fieldCount > 0 ? list->fieldCount : 1;

        if (field && list->fieldCount == 0) {
            CATTO_FREE(field);

            return catto_asTypedNumber(-1);
        }

        for (catto_Count i = 0; i < list->length; i++) {
            if (field && !catto_stringsEqual(field, list->fields[i % fieldCount])) {
                continue;
            }

            catto_TypedValue item = list->values[i];

            if (catto_asNumber(catto_binary_equal(context, item, searchValue))) {
                CATTO_FREE(field);

                return catto_asTypedNumber(i / fieldCount);
            }
        }

        CATTO_FREE(field);

        return catto_asTypedNumber(-1);
    }

    catto_Char* string = catto_asString(sequence);
    catto_Char* searchString = catto_asString(searchValue);
    catto_Int index = -1;

    for (catto_Count i = 0; i < catto_stringLength(string); i++) {
        if (catto_stringStartsWith(string + i, searchString)) {
            index = i;
            break;
        }
    }

    CATTO_FREE(string);
    CATTO_FREE(searchString);

    return catto_asTypedNumber(index);
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

catto_TypedValue catto_function_slicer(catto_Context* context, catto_SlicingMethod method, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Char* result = catto_copyString("");
    catto_Int currentLength = catto_stringLength(value);
    catto_Int startIndex = 0;
    catto_Int endIndex = currentLength;
    catto_Int length = 0;

    if (method == CATTO_SLICING_METHOD_LEFT) {
        endIndex = catto_asNumber(catto_evalNextArg(context));
    } else if (method == CATTO_SLICING_METHOD_RIGHT) {
        startIndex = currentLength - catto_asNumber(catto_evalNextArg(context));
    } else {
        startIndex = catto_asNumber(catto_evalNextArg(context));
        endIndex = startIndex + catto_asNumber(catto_evalNextArg(context));
    }

    if (endIndex > currentLength) {
        endIndex = currentLength;
    }

    for (catto_Int i = startIndex; i < endIndex; i++) {
        if (i < 0) {
            continue;
        }

        result = catto_appendCharToString(result, value[i]);
    }

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(result);

    return returnValue;
}

catto_TypedValue catto_function_left(catto_Context* context, catto_DataType returnType) {
    return catto_function_slicer(context, CATTO_SLICING_METHOD_LEFT, returnType);
}

catto_TypedValue catto_function_right(catto_Context* context, catto_DataType returnType) {
    return catto_function_slicer(context, CATTO_SLICING_METHOD_RIGHT, returnType);
}

catto_TypedValue catto_function_mid(catto_Context* context, catto_DataType returnType) {
    return catto_function_slicer(context, CATTO_SLICING_METHOD_MID, returnType);
}

catto_TypedValue catto_function_trimmer(catto_Context* context, catto_Bool trimLeft, catto_Bool trimRight, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Count currentLength = catto_stringLength(value);
    catto_Char* result = catto_copyString("");

    catto_Count startIndex = 0;
    catto_Count endIndex = currentLength;

    while (trimLeft && startIndex < currentLength) {
        if (value[startIndex] == ' ') {
            startIndex++;
        } else {
            break;
        }
    }

    while (trimRight && endIndex > 1) {
        if (value[endIndex - 1] == ' ') {
            endIndex--;
        } else {
            break;
        }
    }

    for (catto_Int i = startIndex; i < endIndex; i++) {
        if (i < 0) {
            continue;
        }

        result = catto_appendCharToString(result, value[i]);
    }

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(result);

    return returnValue;
}

catto_TypedValue catto_function_trim(catto_Context* context, catto_DataType returnType) {
    return catto_function_trimmer(context, CATTO_TRUE, CATTO_TRUE, returnType);
}

catto_TypedValue catto_function_ltrim(catto_Context* context, catto_DataType returnType) {
    return catto_function_trimmer(context, CATTO_TRUE, CATTO_FALSE, returnType);
}

catto_TypedValue catto_function_rtrim(catto_Context* context, catto_DataType returnType) {
    return catto_function_trimmer(context, CATTO_FALSE, CATTO_TRUE, returnType);
}

catto_TypedValue catto_function_lpad(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Int minLength = catto_asNumber(catto_evalNextArg(context));
    catto_Char* padding = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString(" ");
    catto_Count paddingLength = catto_stringLength(padding);
    catto_Char* result = catto_copyString("");

    if (paddingLength == 0) {
        CATTO_FREE(padding);

        padding = catto_copyString(" ");
        paddingLength = 1;
    }

    for (catto_Int i = catto_stringLength(value); i < minLength; i += paddingLength) {
        result = catto_appendToString(result, padding);
    }

    result = catto_appendToString(result, value);

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(padding);
    CATTO_FREE(result);

    return returnValue;
}

catto_TypedValue catto_function_rpad(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Int minLength = catto_asNumber(catto_evalNextArg(context));
    catto_Char* padding = catto_hasNextArg(context) ? catto_asString(catto_evalNextArg(context)) : catto_copyString(" ");
    catto_Count paddingLength = catto_stringLength(padding);
    catto_Char* result = catto_copyString(value);

    if (paddingLength == 0) {
        CATTO_FREE(padding);

        padding = catto_copyString(" ");
        paddingLength = 1;
    }

    for (catto_Int i = catto_stringLength(value); i < minLength; i += paddingLength) {
        result = catto_appendToString(result, padding);
    }

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(padding);
    CATTO_FREE(result);

    return returnValue;
}

catto_TypedValue catto_function_repeat(catto_Context* context, catto_DataType returnType) {
    catto_Char* value = catto_asString(catto_evalNextArg(context));
    catto_Int repeatCount = catto_asNumber(catto_evalNextArg(context));
    catto_Char* result = catto_copyString("");

    for (catto_Int i = 0; i < repeatCount; i++) {
        result = catto_appendToString(result, value);
    }

    catto_TypedValue returnValue = catto_asTypedString(result);

    CATTO_FREE(value);
    CATTO_FREE(result);

    return returnValue;
}

catto_TypedValue catto_function_random(catto_Context* context, catto_DataType returnType) {
    context->randomSeed = ((context->randomSeed * 10753) + 23279) & CATTO_RANDOM_MASK;

    catto_Float value = context->randomSeed;

    return catto_asTypedNumber(value / CATTO_RANDOM_MASK);
}