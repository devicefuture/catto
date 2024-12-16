catto_Context* catto_newContext() {
    catto_Context* context = CATTO_NEW(catto_Context);

    context->firstCommandHandler = CATTO_NULL;
    context->lastCommandHandler = CATTO_NULL;

    return context;
}

void catto_addCommand(catto_Context* context, catto_Char* name, catto_CommandHandlerFunction function) {
    catto_CommandHandler* commandHandler = CATTO_NEW(catto_CommandHandler);

    commandHandler->name = name;
    commandHandler->function = function;
    commandHandler->nextCommandHandler = CATTO_NULL;

    if (!context->firstCommandHandler) {
        context->firstCommandHandler = commandHandler;
    }

    if (context->lastCommandHandler) {
        context->lastCommandHandler->nextCommandHandler = commandHandler;
    } else {
        context->lastCommandHandler = commandHandler;
    }
}

void catto_command_print(catto_Context* context) {
    CATTO_LOG("Print\n");

    // TODO: Implement actual printing
}

void catto_addContextStandardCommands(catto_Context* context) {
    catto_addCommand(context, "print", &catto_command_print);
}