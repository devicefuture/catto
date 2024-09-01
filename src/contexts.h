catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->isInitialised = CATTO_FALSE;

    return context;
}