catto_Char* catto_operators[] = {
    "+", "-", "*", "/", "div", "mod", "^", "&", "|", "~",
    "!=", "<=", ">=", "=", "<", ">",
    "and", "or", "xor", "not",
    ";",
    CATTO_NULL
};

catto_Char** catto_operatorPrecedence[] = {
    (catto_Char*[]) {"and", "or", "xor", CATTO_NULL},
    (catto_Char*[]) {"!=", "<=", ">=", "=", "<", ">", CATTO_NULL},
    (catto_Char*[]) {"+", "-", CATTO_NULL},
    (catto_Char*[]) {"*", "/", "div", "mod", CATTO_NULL},
    (catto_Char*[]) {"^", CATTO_NULL},
    (catto_Char*[]) {"&", "|", "~", CATTO_NULL},
    (catto_Char*[]) {";", CATTO_NULL},
    CATTO_NULL
};

catto_Char* catto_unaryOperators[] = {
    "+",
    "-",
    "not",
    CATTO_NULL
};

#define CATTO_UNARY_NUMERIC_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue value) { \
        return catto_asTypedNumber(operator catto_asNumber(value)); \
    }

#define CATTO_UNARY_INTEGER_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue value) { \
        return catto_asTypedNumber(operator (catto_Int)catto_asNumber(value)); \
    }

#define CATTO_BINARY_NUMERIC_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber(catto_asNumber(a) operator catto_asNumber(b)); \
    }

#define CATTO_BINARY_INTEGER_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber((catto_Int)catto_asNumber(a) operator (catto_Int)catto_asNumber(b)); \
    }

#define CATTO_BINARY_LOGICAL_OPERATOR(name, operator) catto_TypedValue name(catto_TypedValue a, catto_TypedValue b) { \
        return catto_asTypedNumber(((catto_Int)catto_asNumber(a) operator (catto_Int)catto_asNumber(b)) ? 1 : 0); \
    }

CATTO_UNARY_NUMERIC_OPERATOR(catto_unary_add, +);
CATTO_UNARY_NUMERIC_OPERATOR(catto_unary_subtract, -);
CATTO_UNARY_INTEGER_OPERATOR(catto_unary_bitwiseNot, ~);
CATTO_UNARY_INTEGER_OPERATOR(catto_unary_not, !);

CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_add, +);
CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_subtract, -);
CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_multiply, *);
CATTO_BINARY_NUMERIC_OPERATOR(catto_binary_divide, /);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_integerDivide, /);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_modulo, %);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitwiseAnd, &);
CATTO_BINARY_INTEGER_OPERATOR(catto_binary_bitwiseOr, |);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_notEqual, !=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_lessThanOrEqual, <=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_greaterThanOrEqual, >=);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_equal, ==);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_lessThan, <);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_greaterThan, >);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_and, &&);
CATTO_BINARY_LOGICAL_OPERATOR(catto_binary_or, ||);

catto_TypedValue catto_binary_power(catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(catto_power(catto_asNumber(a), (catto_Int)catto_asNumber(b)));
}

catto_TypedValue catto_binary_xor(catto_TypedValue a, catto_TypedValue b) {
    return catto_asTypedNumber(((catto_Int)catto_asNumber(a) ^ (catto_Int)catto_asNumber(b)) ? 1 : 0);
}

catto_OperatorMapping catto_operatorMappings[] = {
    {"+", catto_unary_add, catto_binary_add},
    {"-", catto_unary_subtract, catto_binary_subtract},
    {"*", CATTO_NULL, catto_binary_multiply},
    {"/", CATTO_NULL, catto_binary_divide},
    {"div", CATTO_NULL, catto_binary_integerDivide},
    {"mod", CATTO_NULL, catto_binary_modulo},
    {"^", CATTO_NULL, catto_binary_power},
    {"&", CATTO_NULL, catto_binary_bitwiseAnd},
    {"|", CATTO_NULL, catto_binary_bitwiseOr},
    {"~", catto_unary_bitwiseNot, CATTO_NULL},
    {"!=", CATTO_NULL, catto_binary_notEqual},
    {"<=", CATTO_NULL, catto_binary_lessThanOrEqual},
    {">=", CATTO_NULL, catto_binary_greaterThanOrEqual},
    {"=", CATTO_NULL, catto_binary_equal},
    {"<", CATTO_NULL, catto_binary_lessThan},
    {">", CATTO_NULL, catto_binary_greaterThan},
    {"and", CATTO_NULL, catto_binary_and},
    {"or", CATTO_NULL, catto_binary_or},
    {"xor", CATTO_NULL, catto_binary_xor},
    {"not", catto_unary_not, CATTO_NULL},
    {CATTO_NULL, CATTO_NULL, CATTO_NULL}
};