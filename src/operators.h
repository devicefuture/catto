catto_Char* catto_operators[] = {
    "+", "-", "*", "/", "^", "div", "mod", "&", "|", "~",
    "!=", "<=", ">=", "=", "<", ">",
    "and", "or", "xor", "not",
    ";",
    CATTO_NULL
};

catto_Char** catto_operatorPrecedence[] = {
    (catto_Char*[]) {"+", "-", CATTO_NULL},
    (catto_Char*[]) {"*", "/", "div", "mod", CATTO_NULL},
    (catto_Char*[]) {"^", CATTO_NULL},
    (catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    (catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    (catto_Char*[]) {"!=", "<=", ">=", "=", "<", ">", CATTO_NULL},
    (catto_Char*[]) {"and", "or", "xor", CATTO_NULL},
    (catto_Char*[]) {";", CATTO_NULL},
    CATTO_NULL
};

catto_Char* catto_unaryOperators[] = {
    "+",
    "-",
    "not",
    CATTO_NULL
};

catto_TypedValue catto_unary_add(catto_TypedValue value) {
    return catto_asTypedNumber(catto_asNumber(value));
}

catto_TypedValue catto_binary_add(catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(catto_asNumber(a) + catto_asNumber(b));
}

catto_TypedValue catto_unary_subtract(catto_TypedValue value) {
    return catto_asTypedNumber(-catto_asNumber(value));
}

catto_TypedValue catto_binary_subtract(catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(catto_asNumber(a) - catto_asNumber(b));
}

catto_TypedValue catto_unary_not(catto_TypedValue value) {
    return catto_asTypedNumber(catto_asNumber(value) ? 0 : 1);
}

catto_OperatorMapping catto_operatorMappings[] = {
    {"+", catto_unary_add, catto_binary_add},
    {"-", catto_unary_subtract, catto_binary_subtract},
    {"not", catto_unary_not, CATTO_NULL},
    {CATTO_NULL, CATTO_NULL, CATTO_NULL}
};