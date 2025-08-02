#ifndef CATTOX_NO_TEST

void cattox_test_hello(catto_Context* context) {
    CATTO_LOG("Hi there!\n");
}

void cattox_test_add(catto_Context* context) {
    catto_Float a = catto_asNumber(catto_evalNextArg(context));
    catto_Float b = catto_asNumber(catto_evalNextArg(context));
    catto_Char* identifier = catto_getIdentifierName(catto_getNextArg(context));

    if (!identifier) {
        context->errorState = CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE;
        return;
    }

    catto_setVariable(context, identifier, catto_asTypedNumber(a + b));
}

void cattox_test_init(catto_Context* context) {
    cattox_Extension* extension = cattox_newExtension(context, "test");

    cattox_addExtensionCommand(extension, "hello", cattox_test_hello);
    cattox_addExtensionCommand(extension, "add", cattox_test_add);
}

#endif