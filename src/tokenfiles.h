void catto_growTokenFile(catto_Context* context, catto_Count amount) {
    context->tokenFileSize += amount;
    context->tokenFile = (catto_Char*)catto_safeRealloc(context->tokenFile, context->tokenFileSize);
}

void catto_appendCharToTokenFile(catto_Context* context, catto_Char c) {
    catto_growTokenFile(context, 1);

    context->tokenFile[context->tokenFileSize - 1] = c;
}

void catto_appendStringToTokenFile(catto_Context* context, const catto_Char* string, catto_Bool nullTerminated) {
    while (*string) {
        catto_appendCharToTokenFile(context, *string);

        string++;
    }

    if (nullTerminated) {
        catto_appendCharToTokenFile(context, '\0');
    }
}

// Counts are encoded as LEB128
// @source reference https://en.wikipedia.org/wiki/LEB128
// @licence ccbysa4
void catto_appendCountToTokenFile(catto_Context* context, catto_Count value) {
    do {
        catto_Char byte = value & 0x7F;

        value >>= 7;

        if (value != 0) {
            byte |= 0x80;
        }

        catto_appendCharToTokenFile(context, byte);
    } while (value != 0);
}

void catto_generateTokenFile(catto_Context* context) {
    context->tokenFile = (catto_Char*)catto_safeRealloc(context->tokenFile, 4);
    context->tokenFileSize = 4;

    context->tokenFile[0] = '\0';
    context->tokenFile[1] = 'A';
    context->tokenFile[2] = 'T';
    context->tokenFile[3] = 0; // Format version number

    catto_appendStringToTokenFile(context, "DEFN", CATTO_FALSE);

    catto_appendCountToTokenFile(context, context->tokenDefinitionsCount);

    for (catto_Count i = 0; i < context->tokenDefinitionsCount; i++) {
        catto_appendStringToTokenFile(context, context->tokenDefinitions[i], CATTO_TRUE);
    }

    catto_appendStringToTokenFile(context, "INDX", CATTO_FALSE);

    catto_appendCountToTokenFile(context, context->tokenIndexesCount);

    for (catto_Count i = 0; i < context->tokenIndexesCount; i++) {
        catto_appendCountToTokenFile(context, context->tokenIndexes[i]);
    }

    catto_appendCharToTokenFile(context, '\0');
}
catto_Bool catto_isTokenFile(catto_Context* context) {
    return context->tokenFileSize >= 4 && catto_memoryEquals(context->tokenFile, "\0AT\0", 4);
}

// Counts are encoded as LEB128
// @source reference https://en.wikipedia.org/wiki/LEB128
// @licence ccbysa4
catto_Count catto_readCountFromTokenFile(catto_Context* context, catto_Count* index) {
    catto_Count result = 0;
    catto_Count shift = 0;
    catto_Count byte;

    do {
        if (*index > context->tokenFileSize) {
            return -1;
        }

        byte = context->tokenFile[(*index)++];

        result |= (byte & 0b01111111) << shift;
        shift += 7;
    } while ((byte & 0b10000000) != 0);

    return result;
}

catto_Char* catto_readStringFromTokenFile(catto_Context* context, catto_Count* index) {
    catto_Char* string = catto_copyString("");

    while (CATTO_TRUE) {
        if (*index > context->tokenFileSize) {
            CATTO_FREE(string);

            return CATTO_NULL;
        }

        if (context->tokenFile[*index] == '\0') {
            (*index)++;
            break;
        }

        string = catto_appendCharToString(string, context->tokenFile[(*index)++]);
    }

    return string;
}

catto_Char* catto_parseTokenFile(catto_Context* context) {
    for (catto_Count i = 0; i < context->tokenDefinitionsCount; i++) {
        CATTO_FREE(context->tokenDefinitions[i]);
    }

    context->tokenDefinitions = (catto_Char**)catto_safeRealloc(context->tokenDefinitions, 0);
    context->tokenDefinitionsCount = 0;
    context->tokenIndexes = (catto_Count*)catto_safeRealloc(context->tokenIndexes, 0);
    context->tokenIndexesCount = 0;

    if (!catto_isTokenFile(context)) {
        return CATTO_NULL;
    }

    catto_Count index = 4;
    catto_Char* code = catto_copyString("");

    while (index < context->tokenFileSize) {
        if (context->tokenFile[index] == '\0') {
            break;
        }

        if (index + 4 >= context->tokenFileSize) {
            return CATTO_NULL;
        }

        if (catto_memoryEquals(context->tokenFile + index, "DEFN", 4)) {
            index += 4;

            catto_Count definitionsCount = catto_readCountFromTokenFile(context, &index);

            if (definitionsCount == -1) {
                CATTO_FREE(code);
                return CATTO_NULL;
            }

            context->tokenDefinitionsCount = definitionsCount;
            context->tokenDefinitions = (catto_Char**)catto_safeRealloc(context->tokenDefinitions, sizeof(catto_Char*) * definitionsCount);

            for (catto_Count i = 0; i < definitionsCount; i++) {
                catto_Char* value = catto_readStringFromTokenFile(context, &index);

                if (!value) {
                    CATTO_FREE(code);
                    return CATTO_NULL;
                }

                context->tokenDefinitions[i] = value;
            }

            continue;
        }

        if (catto_memoryEquals(context->tokenFile + index, "INDX", 4)) {
            index += 4;

            catto_Count indexesCount = catto_readCountFromTokenFile(context, &index);

            if (indexesCount == -1) {
                CATTO_FREE(code);
                return CATTO_NULL;
            }

            for (catto_Count i = 0; i < indexesCount; i++) {
                catto_Count tokenIndex = catto_readCountFromTokenFile(context, &index);

                if (tokenIndex == -1 || tokenIndex >= context->tokenDefinitionsCount) {
                    CATTO_FREE(code);
                    return CATTO_NULL;
                }

                code = catto_appendToString(code, context->tokenDefinitions[tokenIndex]);
                code = catto_appendCharToString(code, ' ');
            }
        }
    }

    return code;
}