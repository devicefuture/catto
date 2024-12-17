#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <catto-config.h>
#include <catto.h>

// #define DEBUG_MEMORY

typedef struct Line {
    unsigned int lineNumber;
    char* code;
    struct Line* nextLine;
} Line;

Line* firstLine = NULL;

char* assembleLines() {
    char* code = catto_copyString("");
    Line* currentLine = firstLine;

    code[0] = '\0';

    while (currentLine) {
        code = catto_appendToString(code, currentLine->code);

        currentLine = currentLine->nextLine;
    }

    return code;
}

int main(int argc, char* argv[]) {
    printf("Welcome to catto\n");
    printf("Ready\n");

    catto_Context* context = catto_newContext();

    catto_addContextStandardCommands(context);

    char* lineString = NULL;

    while (true) {
        size_t size;

        #ifdef DEBUG_MEMORY
            printf("Memory usage: %d\n", mallinfo2().uordblks);
        #endif

        getline(&lineString, &size, stdin);

        if (catto_stringsEqual(lineString, "run\n")) {
            char* code = assembleLines();

            catto_load(context, code);

            if (context->errorState == CATTO_ERROR_STATE_UNEXPECTED_TOKEN) {
                if (context->subjectLineNumber > 0) {
                    printf("Unexpected token at line %d\n", context->subjectLineNumber);                    
                } else {
                    printf("Unexpected token\n");
                }
            } else {
                catto_run(context);
            }

            free(code);

            printf("Ready\n");

            continue;
        }

        if (catto_stringsEqual(lineString, "list\n")) {
            Line* currentLine = firstLine;

            while (currentLine) {
                printf("%s", currentLine->code);

                currentLine = currentLine->nextLine;
            }

            continue;
        }

        catto_Token* firstToken = catto_tokenise(context, lineString);

        if (firstToken && firstToken->type == CATTO_TOKEN_TYPE_LINE_NUMBER) {
            Line* line = malloc(sizeof(Line));

            line->lineNumber = firstToken->value.asLineNumber;
            line->code = catto_copyString(lineString);
            line->nextLine = NULL;

            Line* currentLine = firstLine;

            if (firstToken->nextToken && firstToken->nextToken->type == CATTO_TOKEN_TYPE_NEXT_LINE) {
                if (firstLine && firstLine->lineNumber == line->lineNumber) {
                    Line* originalFirstLine = firstLine;

                    firstLine = firstLine->nextLine;

                    free(originalFirstLine->code);
                    free(originalFirstLine);

                    continue;
                }

                Line* previousLine = NULL;

                while (currentLine) {
                    if (previousLine && currentLine->lineNumber == line->lineNumber) {
                        previousLine->nextLine = currentLine->nextLine;

                        free(currentLine->code);
                        free(currentLine);

                        break;
                    }

                    previousLine = currentLine;
                    currentLine = currentLine->nextLine;
                }                

                continue;
            }

            if (firstLine) {

                while (currentLine) {
                    if (currentLine->lineNumber == line->lineNumber) {
                        free(currentLine->code);

                        currentLine->code = line->code;

                        break;
                    }

                    if (currentLine->lineNumber > line->lineNumber) {
                        char* lineCode = line->code;
                        unsigned int lineNumber = line->lineNumber;

                        line->lineNumber = currentLine->lineNumber;
                        line->code = currentLine->code;
                        line->nextLine = currentLine->nextLine;

                        currentLine->lineNumber = lineNumber;
                        currentLine->code = lineCode;
                        currentLine->nextLine = line;

                        break;
                    }

                    if (!currentLine->nextLine) {
                        currentLine->nextLine = line;

                        break;
                    }

                    currentLine = currentLine->nextLine;
                }
            } else {
                firstLine = line;
                line->nextLine = NULL;
            }

            continue;
        }
    }
}