#ifndef CATTO_CUSTOM_PRINT_COMMAND
void catto_command_print(catto_Context* context) {
    catto_Bool appendFlag = CATTO_FALSE;

    while (catto_hasNextArg(context)) {
        catto_AstNode* arg = catto_getNextArg(context);
        catto_Char* string = catto_asString(catto_evalExpression(context, arg));

        if (!catto_hasNextArg(context) && catto_hasAppendFlag(arg)) {
            appendFlag = CATTO_TRUE;
        }

        CATTO_LOG(string);
        CATTO_FREE(string);

        if (catto_hasNextArg(context)) {
            CATTO_LOG(" ");
        }
    }

    if (!appendFlag) {
        CATTO_LOG("\n");
    }
}
#endif

void catto_command_scrawl(catto_Context* context) {
    context->scrawlMode = CATTO_TRUE;
}

void catto_command_noscrawl(catto_Context* context) {
    context->scrawlMode = CATTO_FALSE;
}

void catto_command_extload(catto_Context* context) {
    catto_Char* nameIdentifier = catto_getIdentifierName(catto_getNextArg(context));
    catto_Char* name = nameIdentifier ? catto_copyString(nameIdentifier) : catto_asString(catto_evalNextArg(context));

    cattox_Extension* extension = cattox_findExtension(context, name, CATTO_FALSE);

    if (!extension) {
        context->errorState = CATTO_ERROR_STATE_UNKNOWN_EXTENSION;
        CATTO_FREE(name);
        return;
    }

    if (extension->alias) {
        CATTO_FREE(extension->alias);
    }

    catto_AstNode* aliasArg = catto_getNextArg(context);

    if (aliasArg) {
        catto_Char* aliasIdentifier = catto_copyString(catto_getIdentifierName(aliasArg));
        catto_Char* alias = aliasIdentifier ? aliasIdentifier : catto_asString(catto_evalNextArg(context));

        extension->alias = catto_copyString(alias);

        CATTO_FREE(alias);
    } else {
        extension->alias = catto_copyString(name);
    }

    CATTO_FREE(name);
}