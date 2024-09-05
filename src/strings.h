#ifndef CATTO_STRINGS_H_
#define CATTO_STRINGS_H_

catto_Count catto_stringLength(catto_Char* string) {
    catto_Count length = 0;

    while (string[length] != '\0') {
        length++;
    }

    return length;
}

catto_Bool catto_stringsEqual(catto_Char* a, catto_Char* b) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (a[i] == b[i]) {
        if (a[i] == '\0') {
            if (b[i] == '\0') {
                return CATTO_TRUE;
            }

            return CATTO_FALSE;
        }

        if (b[i] == '\0') {
            return CATTO_FALSE;
        }

        i++;
    }

    return CATTO_FALSE;
}

catto_Bool catto_stringStartsWith(catto_Char* a, catto_Char* b) {
    catto_Count i = 0;

    if (a == b) {
        return CATTO_TRUE;
    }

    while (a[i] == b[i]) {
        if (a[i] == '\0') {
            if (b[i] == '\0') {
                return CATTO_TRUE;
            }

            return CATTO_FALSE;
        }

        i++;
    }

    if (b[i] == '\0') {
        return CATTO_TRUE;
    }

    return CATTO_FALSE;
}

#endif