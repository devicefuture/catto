cattox_Extension* cattox_newExtension(catto_Context* context, const catto_Char* name) {
    cattox_Extension* extension = CATTO_NEW(cattox_Extension);

    extension->name = name;
    extension->alias = CATTO_NULL;
    extension->firstCommandHandler = CATTO_NULL;
    extension->lastCommandHandler = CATTO_NULL;
    extension->nextExtension = CATTO_NULL;

    if (!context->firstExtension) {
        context->firstExtension = extension;
    }

    if (context->lastExtension) {
        context->lastExtension->nextExtension = extension;
    }

    context->lastExtension = extension;

    return extension;
}

cattox_Extension* cattox_findExtension(catto_Context* context, const catto_Char* name, catto_Bool useAlias) {
    cattox_Extension* extension = context->firstExtension;

    while (extension) {
        if (!useAlias && catto_stringsEqualCaseInsensitive(extension->name, name)) {
            return extension;
        }

        if (useAlias && catto_stringsEqualCaseInsensitive(extension->alias, name)) {
            return extension;
        }

        extension = extension->nextExtension;
    }
}

catto_CommandHandler* cattox_findCommandHandlerInExtension(cattox_Extension* extension, const catto_Char* command) {
    catto_CommandHandler* commandHandler = extension->firstCommandHandler;

    while (commandHandler) {
        if (catto_stringsEqualCaseInsensitive(commandHandler->name, command)) {
            return commandHandler;
        }

        commandHandler = commandHandler->nextCommandHandler;
    }
}

void cattox_addExtensionCommand(cattox_Extension* extension, const catto_Char* name, catto_CommandHandlerFunction function) {
    catto_CommandHandler* commandHandler = CATTO_NEW(catto_CommandHandler);

    commandHandler->name = name;
    commandHandler->function = function;
    commandHandler->nextCommandHandler = CATTO_NULL;

    if (!extension->firstCommandHandler) {
        extension->firstCommandHandler = commandHandler;
    }

    if (extension->lastCommandHandler) {
        extension->lastCommandHandler->nextCommandHandler = commandHandler;
    }

    extension->lastCommandHandler = commandHandler;
}