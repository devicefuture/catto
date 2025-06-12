catto_Float catto_power(catto_Float base, catto_Int power) {
    if (power == 0) {
        return 1;
    }

    if (power < 0) {
        base = 1 / base;
        power *= -1;
    }

    catto_Float result = base;

    while (power > 1) {
        result *= base;
        power--;
    }

    return result;
}

// @source https://stackoverflow.com/a/49991852
catto_Float catto_sqrt(catto_Float value) {
    catto_Float result = 1;

    if (value < 0) {
        return CATTO_NAN;
    }

    for (catto_Count i = 1; i <= CATTO_SQRT_ITERATIONS; i++) {
        result -= ((result * result) - value) / (2 * result);
    }

    return result;
}

catto_Float catto_roundToPrecision(catto_Float number, catto_Count precision) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    catto_Int integralPart = number;
    catto_Count integralDigits = 0;

    while (integralPart > 0) {
        integralPart /= 10;
        integralDigits++;
    }

    precision -= integralDigits;

    if (precision < 0) {
        precision = 0;
    }

    catto_Int multiplier = catto_power(10, precision);

    number += 0.5 * catto_power(10, -precision);

    if (isNegative) {
        number *= -1;
    }

    return (catto_Float)((catto_Int)(number * multiplier)) / multiplier;
}

catto_Char* catto_numberToString(catto_Float number) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    if (number != number) {
        return catto_copyString("NaN");
    }

    if (number == CATTO_INFINITY) {
        return catto_copyString(isNegative ? "-Infinity" : "Infinity");
    }

    catto_Int exponent = 0;
    catto_Char* string = catto_copyString("");
    catto_Count precisionLeft = CATTO_MAX_PRECISION;

    if (number > 0) {
        if (number < catto_power(10, -CATTO_MAX_PRECISION + 1)) {
            while (number < 1 - catto_power(10, -CATTO_MAX_PRECISION)) {
                number *= 10;
                exponent--;
            }
        }

        if (number > catto_power(10, CATTO_MAX_PRECISION - 1)) {
            while (number > 10 + catto_power(10, -CATTO_MAX_PRECISION)) {
                number /= 10;
                exponent++;
            }
        }
    }

    number = catto_roundToPrecision(number, CATTO_MAX_PRECISION);

    catto_Int integralPart = number;

    number += 0.1 * catto_power(10, -precisionLeft);
    number -= integralPart; // Now fractional part

    do {
        string = catto_appendCharToString(string, (catto_Char)('0' + (integralPart % 10)));

        integralPart /= 10;
        precisionLeft--;
    } while (integralPart > 0);

    if (isNegative) {
        string = catto_appendCharToString(string, '-');
    }

    catto_reverseString(string);

    catto_Count trailingZeroes = 0;
    catto_Bool anyDigitsInFractionalPart = CATTO_FALSE;

    if (number > CATTO_EPSILON && precisionLeft > 0) {
        string = catto_appendCharToString(string, '.');

        while (number > CATTO_EPSILON && precisionLeft > 0) {
            number *= 10;

            catto_Char digit = number;

            if (digit == 0) {
                trailingZeroes++;
            } else {
                trailingZeroes = 0;
                anyDigitsInFractionalPart = CATTO_TRUE;
            }

            string = catto_appendCharToString(string, (catto_Char)('0' + digit));

            number -= digit;
            precisionLeft--;
        }
    }

    if (trailingZeroes > 0) {
        if (!anyDigitsInFractionalPart) {
            trailingZeroes++;
        }

        catto_Count newStringLength = catto_stringLength(string) - trailingZeroes;

        string = (catto_Char*)CATTO_REALLOC(string, newStringLength + 1);
        string[newStringLength] = '\0';
    }

    if (exponent != 0) {
        string = catto_appendCharToString(string, 'E');

        if (exponent > 0) {
            string = catto_appendCharToString(string, '+');
        }

        catto_Char* exponentString = catto_numberToString(exponent);

        string = catto_appendToString(string, exponentString);

        CATTO_FREE(exponentString);
    }

    return string;
}

catto_Char* catto_numberToBaseString(catto_Float number, catto_Count base) {
    catto_Bool isNegative = CATTO_FALSE;

    if (number < 0) {
        isNegative = CATTO_TRUE;
        number *= -1;
    }

    if (number != number) {
        return catto_copyString("NaN");
    }

    if (number == CATTO_INFINITY) {
        return catto_copyString(isNegative ? "-Infinity" : "Infinity");
    }

    catto_Char* string = catto_copyString("");

    do {
        catto_Char digit = (catto_Int)number % base;

        if (digit < 10) {
            digit += '0';
        } else if (digit < 16) {
            digit += 'a' - 10;
        } else {
            digit = '?';
        }

        string = catto_appendCharToString(string, digit);

        number /= base;
    } while (number >= 1);

    if (isNegative) {
        string = catto_appendCharToString(string, '-');
    }

    catto_reverseString(string);

    return string;
}