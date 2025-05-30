catto_TypedValue catto_function_true(catto_Context* context, catto_DataType returnType) {
    return catto_asTypedNumber(1);
}

catto_TypedValue catto_function_false(catto_Context* context, catto_DataType returnType) {
    return catto_asTypedNumber(0);
}