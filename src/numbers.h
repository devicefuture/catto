// @source https://stackoverflow.com/a/77133316
// TODO: Replace with better approximation
catto_Float catto_ln(catto_Float value) {
    if (value < 0) {
        return CATTO_NAN;
    }

    if (value < 0.5) {
        return -catto_ln(1.0 / value);
    }

    catto_Float multiplier = (value - 1) / (value + 1);
    catto_Float result = 0;
    catto_Float term = multiplier;

    for (catto_Count i = 1; i <= 100; i += 2) {
        result += term / i;
        term *= multiplier * multiplier;
    }

    return result * 2.0;
}

catto_Float catto_log(catto_Float value) {
    return catto_ln(value) / 2.3025850929940457;
}

catto_Float catto_log2(catto_Float value) {
    return catto_ln(value) / 0.6931471805599453;
}

// @source https://math.stackexchange.com/a/4581483
catto_Float catto_exp2(catto_Float value) {
    catto_Int integralPart = value;
    catto_Float fractionalPart = value - integralPart;
    catto_Float multiplier = 1.0 + (27.704226690769845416 / (4.8416702244134115171 - fractionalPart)) - (0.48942480030516666506 * fractionalPart) - 5.7220391320516093836;

    return (catto_Float)(1 << integralPart) * multiplier;
}

catto_Float catto_power(catto_Float base, catto_Float power) {
    if (power == 0) {
        return 1;
    }

    if (power < 0) {
        base = 1 / base;
        power *= -1;
    }

    if (power != (catto_Int)power) {
        return catto_exp2(power * catto_log2(base));
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

catto_Float catto_fromRadians(catto_Float value, catto_TrigMode trigMode) {
    switch (trigMode) {
        case CATTO_TRIG_MODE_RADIANS: return value;
        case CATTO_TRIG_MODE_DEGREES: return value / (CATTO_PI / 180);
        case CATTO_TRIG_MODE_GRADIANS: return value / (CATTO_PI / 200);
        case CATTO_TRIG_MODE_TURNS: return value / (2 * CATTO_PI);
    }

    return value;
}

catto_Float catto_toRadians(catto_Float value, catto_TrigMode trigMode) {
    switch (trigMode) {
        case CATTO_TRIG_MODE_RADIANS: return value;
        case CATTO_TRIG_MODE_DEGREES: return value * (CATTO_PI / 180);
        case CATTO_TRIG_MODE_GRADIANS: return value * (CATTO_PI / 200);
        case CATTO_TRIG_MODE_TURNS: return value * (2 * CATTO_PI);
    }

    return value;
}

catto_Float catto_floatMod(catto_Float a, catto_Float b) {
    catto_Float divisionResult = a / b;
    catto_Float flooredResult = (catto_Int)(divisionResult < 0 ? divisionResult - 1 : divisionResult);

    return a - (flooredResult * b);
}

catto_Float catto_sin(catto_Float value) {
    return catto_cos(value - (CATTO_PI / 2));
}

// @source https://stackoverflow.com/a/2284969
catto_Float catto_cos(catto_Float value) {
    catto_Float partResult = 1;
    catto_Float result = 1;

    value = catto_floatMod(value, 2 * CATTO_PI);

    for (catto_Count i = 1; i <= CATTO_COS_ITERATIONS; i++) {
        partResult = (-partResult * value * value) / (((2 * i) - 1) * 2 * i);
        result += partResult;
    }

    return catto_roundToPrecision(result, 14);
}

catto_Float catto_tan(catto_Float value) {
    return catto_sin(value) / catto_cos(value);
}

catto_Float catto_asin(catto_Float value) {
    if (value < -1 || value > 1) {
        return CATTO_NAN;
    }

    return catto_atan(value / catto_sqrt(1 - (value * value)));
}

catto_Float catto_acos(catto_Float value) {
    if (value < -1 || value > 1) {
        return CATTO_NAN;
    }

    if (value == -1) {
        return CATTO_PI;
    }

    catto_Float result = catto_atan(catto_sqrt(1 - (value * value)) / value);

    return result < 0 ? result + CATTO_PI : result;
}

catto_Float catto_atan(catto_Float value) {
    const catto_Float results[] = {
        0,
        0.01745506492, 0.03492076949, 0.05240777928, 0.06992681194,
        0.08748866352, 0.10510423526, 0.12278456090, 0.14054083470,
        0.15838444032, 0.17632698070, 0.19438030913, 0.21255656167,
        0.23086819112, 0.24932800284, 0.26794919243, 0.28674538575,
        0.30573068145, 0.32491969623, 0.34432761328, 0.36397023426,
        0.38386403503, 0.40402622583, 0.42447481620, 0.44522868530,
        0.46630765815, 0.48773258856, 0.50952544949, 0.53170943166,
        0.55430905145, 0.57735026918, 0.60086061902, 0.62486935190,
        0.64940759319, 0.67450851684, 0.70020753820, 0.72654252800,
        0.75355405010, 0.78128562650, 0.80978403319, 0.83909963117,
        0.86928673781, 0.90040404429, 0.93251508613, 0.96568877480,
        0.99999999999, 1.03553031379, 1.07236871002, 1.11061251482,
        1.15036840722, 1.19175359259, 1.23489715653, 1.27994163219,
        1.32704482162, 1.37638192047, 1.42814800674, 1.48256096851,
        1.53986496381, 1.60033452904, 1.66427948235, 1.73205080756,
        1.80404775527, 1.88072646534, 1.96261050550, 2.05030384157,
        2.14450692050, 2.24603677390, 2.35585236582, 2.47508685341,
        2.60508906469, 2.74747741945, 2.90421087767, 3.07768353717,
        3.27085261848, 3.48741444384, 3.73205080756, 4.01078093353,
        4.33147587428, 4.70463010947, 5.14455401597, 5.67128181961,
        6.31375151467, 7.11536972238, 8.14434642797, 9.51436445422,
        11.4300523027, 14.3006662567, 19.0811366877, 28.6362532829,
        57.2899616307, 171.869884892, 1.0 / 0.0
    };

    const catto_Count resultCount = sizeof(results) / sizeof(results[0]);

    catto_Float result = 90;
    catto_Bool negative = value < 0;

    if (negative) {
        value *= -1;
    }

    for (catto_Count i = 0; i < resultCount - 1; i++) {
        if (value >= results[i] - CATTO_EPSILON && value < results[i + 1] + CATTO_EPSILON) {
            result = i + ((value - results[i]) / (results[i + 1] - results[i]));
            break;
        }
    }

    result = catto_toRadians(catto_roundToPrecision(result, 10), CATTO_TRIG_MODE_DEGREES);

    if (negative) {
        result *= -1;
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

    number += 0.5 * catto_power(10, -(catto_Int)precision);

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

    number += 0.1 * catto_power(10, -(catto_Int)precisionLeft);
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