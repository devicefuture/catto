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