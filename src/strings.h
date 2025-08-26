#ifndef CATTO_STRINGS_H_
#define CATTO_STRINGS_H_

catto_Count catto_stringLength(const catto_Char* string) {
    catto_Count length = 0;

    while (string[length] != '\0') {
        length++;
    }

    return length;
}

catto_Bool _catto_charsEqual(catto_Char a, catto_Char b, catto_Bool caseInsensitive) {
    if (caseInsensitive) {
        if (a >= 'A' && a <= 'Z') {
            a += 32;
        }

        if (b >= 'A' && b <= 'Z') {
            b += 32;
        }
    }

    return a == b;
}

catto_Bool _catto_stringsEqual(const catto_Char* a, const catto_Char* b, catto_Bool caseInsensitive) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    if (!a || !b) {
        return CATTO_FALSE;
    }

    while (_catto_charsEqual(a[i], b[i], caseInsensitive)) {
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

catto_Bool catto_stringsEqual(const catto_Char* a, const catto_Char* b) {
    return _catto_stringsEqual(a, b, CATTO_FALSE);
}

catto_Bool catto_stringsEqualCaseInsensitive(const catto_Char* a, const catto_Char* b) {
    return _catto_stringsEqual(a, b, CATTO_TRUE);
}

CATTO_THROWS(CATTO_NULL) catto_Char* catto_copyString(const catto_Char* string) {
    catto_Count length = catto_stringLength(string);
    catto_Char* newString = (catto_Char*)CATTO_MALLOC(length + 1); CATTO_MUST_N(newString);

    for (catto_Count i = 0; i < length; i++) {
        newString[i] = string[i];
    }

    newString[length] = '\0';

    return newString;
}

CATTO_THROWS(CATTO_NULL) catto_Char* catto_appendCharToString(catto_Char* string, catto_Char character) {
    catto_Count length = catto_stringLength(string);

    CATTO_SAFE_REALLOC(string, catto_Char*, length + 2, CATTO_MUST_N(CATTO_DEST));

    string[length] = character;
    string[length + 1] = '\0';

    return string;
}

CATTO_THROWS(CATTO_NULL) catto_Char* catto_appendToString(catto_Char* a, const catto_Char* b) {
    catto_Count aLength = catto_stringLength(a);
    catto_Count bLength = catto_stringLength(b);

    CATTO_SAFE_REALLOC(a, catto_Char*, aLength + bLength + 1, CATTO_MUST_N(CATTO_DEST));

    a[aLength + bLength] = '\0';

    for (catto_Count i = 0; i < bLength; i++) {
        a[aLength + i] = b[i];
    }

    return a;
}

CATTO_THROWS(CATTO_NULL) catto_Char* catto_reverseString(catto_Char* string) {
    catto_Char* tempString = catto_copyString(string); CATTO_MUST_N(tempString);
    catto_Count stringLength = catto_stringLength(string);

    for (catto_Count i = 0; i < stringLength; i++) {
        string[stringLength - 1 - i] = tempString[i];
    }

    CATTO_FREE(tempString);

    return string;
}

catto_Bool _catto_stringStartsWith(const catto_Char* a, const catto_Char* b, catto_Bool caseInsensitive) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (_catto_charsEqual(a[i], b[i], caseInsensitive)) {
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

catto_Bool catto_stringStartsWith(const catto_Char* a, const catto_Char* b) {
    return _catto_stringStartsWith(a, b, CATTO_FALSE);
}

catto_Bool catto_stringStartsWithCaseInsensitive(const catto_Char* a, const catto_Char* b) {
    return _catto_stringStartsWith(a, b, CATTO_TRUE);
}

catto_Float catto_unsignedStringToBaseNumber(const catto_Char* string, catto_Count base, catto_Count* charactersEaten) {
    *charactersEaten = 0;

    catto_Count i = 0;
    catto_Float result = 0;

    while (string[i] != '\0') {
        if (
            (string[i] == '0' || string[i] == '1') ||
            (base >= 8 && string[i] >= '2' && string[i] <= '7') ||
            (base >= 10 && string[i] >= '8' && string[i] <= '9')
        ) {
            result *= base;
            result += string[i] - '0';
        } else if (base >= 16 && string[i] >= 'A' && string[i] <= 'F') {
            result *= base;
            result += string[i] - 'A' + 0xA;
        } else if (base >= 16 && string[i] >= 'a' && string[i] <= 'f') {
            result *= base;
            result += string[i] - 'a' + 0xA;
        } else {
            break;
        }

        i++;
    }

    *charactersEaten = i;

    return result;
}

// @source https://stackoverflow.com/a/4392789
catto_Float catto_unsignedStringToNumber(const catto_Char* string, catto_Count* charactersEaten) {
    *charactersEaten = 0;

    catto_Count i = 0;
    catto_Float result = 0;
    catto_Float factor = 1;
    catto_Float exponent = 0;
    catto_Float exponentIsNegative = CATTO_FALSE;
    catto_Bool afterPoint = CATTO_FALSE;
    catto_Bool hadDigit = CATTO_FALSE;
    catto_Bool hadDigitAfterPoint = CATTO_FALSE;
    catto_Bool afterExponentMark = CATTO_FALSE;
    catto_Bool afterExponentSign = CATTO_FALSE;

    if (string[0] == '0') {
        switch (string[1]) {
            case 'b': case 'B': result = catto_unsignedStringToBaseNumber(string + 2, 2, charactersEaten); break;
            case 'o': case 'O': result = catto_unsignedStringToBaseNumber(string + 2, 8, charactersEaten); break;
            case 'x': case 'X': result = catto_unsignedStringToBaseNumber(string + 2, 16, charactersEaten); break;

            default: break;
        }

        if (*charactersEaten > 0) {
            *charactersEaten += 2;

            return result;
        }
    }

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
            hadDigitAfterPoint = CATTO_TRUE;
        } else {
            break;
        }

        i++;
    }

    if (afterPoint && !hadDigit && !hadDigitAfterPoint) {
        return 0;
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

catto_Float catto_stringToNumber(const catto_Char* string, catto_Count* charactersEaten) {
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

catto_Float catto_stringToBaseNumber(const catto_Char* string, catto_Count base, catto_Count* charactersEaten) {
    catto_Bool ateSign = CATTO_FALSE;
    catto_Bool negate = CATTO_FALSE;

    if (string[0] == '+' || string[0] == '-') {
        negate = string[0] == '-';
        ateSign = CATTO_TRUE;
        string += 1;
    }

    catto_Float result = catto_unsignedStringToBaseNumber(string, base, charactersEaten);

    if (ateSign) {
        (*charactersEaten)++;
    }

    if (negate) {
        result *= -1;
    }

    return result;
}

#endif