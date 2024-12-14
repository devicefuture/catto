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
catto_Float catto_stringToPositiveNumber(catto_Char* string, catto_Count* charactersEaten) {
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

#endif