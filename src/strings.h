#ifndef CATTO_STRINGS_H_
#define CATTO_STRINGS_H_

catto_Count catto_stringLength(catto_Char* string) {
    catto_Count length = 0;

    while (string[length] != '\0') {
        length++;
    }

    return length;
}

catto_Bool catto_stringsEqual(catto_Char* a, catto_Char* b) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (a[i] == b[i]) {
        if (a[i] == '\0') {
            if (b[i] == '\0') {
                return CATTO_TRUE;
            }

            return CATTO_FALSE;
        }

        if (b[i] == '\0') {
            return CATTO_FALSE;
        }

        i++;
    }

    return CATTO_FALSE;
}

catto_Char* catto_copyString(catto_Char* string) {
    catto_Count length = catto_stringLength(string);
    catto_Char* newString = CATTO_MALLOC(length + 1);

    for (catto_Count i = 0; i < length; i++) {
        newString[i] = string[i];
    }

    newString[length] = '\0';

    return newString;
}

catto_Char* catto_appendCharToString(catto_Char* string, catto_Char character) {
    catto_Count length = catto_stringLength(string);

    string = CATTO_REALLOC(string, length + 2);
    string[length] = character;
    string[length + 1] = '\0';

    return string;
}

catto_Char* catto_appendToString(catto_Char* a, catto_Char* b) {
    catto_Count aLength = catto_stringLength(a);
    catto_Count bLength = catto_stringLength(b);

    a = CATTO_REALLOC(a, aLength + bLength + 1);
    a[aLength + bLength] = '\0';

    for (catto_Count i = 0; i < bLength; i++) {
        a[aLength + i] = b[i];
    }

    return a;
}

catto_Char* catto_reverseString(catto_Char* string) {
    catto_Char* tempString = catto_copyString(string);
    catto_Count stringLength = catto_stringLength(string);

    for (catto_Count i = 0; i < stringLength; i++) {
        string[stringLength - 1 - i] = tempString[i];
    }

    CATTO_FREE(tempString);

    return string;
}

catto_Bool catto_stringStartsWith(catto_Char* a, catto_Char* b) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (a[i] == b[i]) {
        if (a[i] == '\0') {
            if (b[i] == '\0') {
                return CATTO_TRUE;
            }

            return CATTO_FALSE;
        }

        i++;
    }

    if (b[i] == '\0') {
        return CATTO_TRUE;
    }

    return CATTO_FALSE;
}

// @source https://stackoverflow.com/a/4392789
catto_Float catto_unsignedStringToNumber(catto_Char* string, catto_Count* charactersEaten) {
    *charactersEaten = 0;

    catto_Count i = 0;
    catto_Float result = 0;
    catto_Float factor = 1;
    catto_Float exponent = 0;
    catto_Float exponentIsNegative = CATTO_FALSE;
    catto_Bool afterPoint = CATTO_FALSE;
    catto_Bool hadDigit = CATTO_FALSE;
    catto_Bool afterExponentMark = CATTO_FALSE;
    catto_Bool afterExponentSign = CATTO_FALSE;

    while (string[i] != '\0') {
        catto_Char character = string[i];

        if (character == '.' && !afterPoint) {
            afterPoint = CATTO_TRUE;
        } else if (
            (character == 'e' || character == 'E') &&
            !afterExponentMark && hadDigit
        ) {
            afterExponentMark = CATTO_TRUE;
            hadDigit = CATTO_FALSE;
        } else if (
            (character == '+' || character == '-') &&
            afterExponentMark && !afterExponentSign && !hadDigit
        ) {
            exponentIsNegative = character == '-';
            afterExponentSign = CATTO_TRUE;
        } else if (character >= '0' && character <= '9') {
            catto_Int digit = character - '0';

            if (afterExponentMark) {
                exponent = (exponent * 10.0) + digit;
            } else {
                if (afterPoint) {
                    factor /= 10.0;
                }

                result = (result * 10.0) + digit;
            }

            hadDigit = CATTO_TRUE;
        } else {
            break;
        }

        i++;
    }

    if (!afterExponentMark) {
        exponent = 1;
    }

    if (!exponentIsNegative) {
        if (exponent == 1) {
            // Do nothing
        } else if (exponent == 0) {
            result = 1;
        } else {
            for (catto_Count i = 0; i < exponent; i++) {
                result *= 10.0;
            }
        }
    } else {
        for (catto_Count i = 0; i < exponent; i++) {
            result /= 10.0;
        }
    }

    *charactersEaten = i;

    return result * factor;
}

catto_Float catto_stringToNumber(catto_Char* string, catto_Count* charactersEaten) {
    catto_Bool ateSign = CATTO_FALSE;
    catto_Bool negate = CATTO_FALSE;

    if (string[0] == '+' || string[0] == '-') {
        negate = string[0] == '-';
        ateSign = CATTO_TRUE;
        string += 1;
    }

    catto_Float result = catto_unsignedStringToNumber(string, charactersEaten);

    if (ateSign) {
        (*charactersEaten)++;
    }

    if (negate) {
        result *= -1;
    }

    return result;
}

#endif