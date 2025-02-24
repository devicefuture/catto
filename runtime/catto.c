#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <catto-config.h>
#include <catto.h>

// #define DEBUG_MEMORY

bool interrupted = false;

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
        code = catto_appendCharToString(code, '\n');

        currentLine = currentLine->nextLine;
    }

    return code;
}

bool readLine(char** line) {
    unsigned int i = 0;

    if (*line == NULL) {
        *line = malloc(1);
    } else {
        *line = realloc(*line, 1);
    }

    (*line)[0] = '\0';

    while (true) {
        int c = getchar();

        if (c < 0) {
            continue;
        }

        if (c == '\n') { // Newline
            putc('\n', stdout);

            return true;
        }

        if (c == '\e') { // Escape
            putc('\n', stdout);

            return false;
        }

        if (c == 127) { // Backspace
            if (i == 0) {
                continue;
            }

            *line = realloc(*line, i + 1);

            (*line)[i--] = '\0';

            printf("\b \b");

            continue;
        }

        *line = realloc(*line, i + 2);

        putc(c, stdout);

        (*line)[i++] = c;
        (*line)[i] = '\0';
    }
}

void runCode(catto_Context* context, char* code) {
    interrupted = false;

    catto_load(context, code);

    while (catto_step(context)) {
        if (getchar() == '\e') { // Escape
            interrupted = true;
        }

        if (interrupted) {
            break;
        }
    }

    catto_Char* message = "Unknown error";

    switch (context->errorState) {
        case CATTO_ERROR_STATE_UNEXPECTED_TOKEN: message = "Unexpected token"; break;
        case CATTO_ERROR_STATE_NO_RETURN: message = "Nothing to return to"; break;
        case CATTO_ERROR_STATE_MISMATCHED_OPENING_MARK: message = "Mismatched statement opening mark"; break;
        case CATTO_ERROR_STATE_MISMATCHED_CLOSING_MARK: message = "Mismatched statement closing mark"; break;
        case CATTO_ERROR_STATE_LOOP_CONTROL_OUTSIDE_LOOP: message = "Loop control command was used outside of loop"; break;

        default: break;
    }

    if (context->errorState != CATTO_ERROR_STATE_NONE) {
        if (context->subjectLineNumber > 0) {
            printf("%s at line %d\n", message, context->subjectLineNumber);
        } else {
            printf("%s\n", message);
        }
    }

    free(code);

    printf(interrupted ? "Interrupt\n" : "Ready\n");
}

void inputCommand(catto_Context* context) {
    char* string = catto_asString(catto_evalNextArg(context));

    printf("%s", string);

    free(string);

    catto_AstNode* identifier = catto_getNextArg(context);

    if (identifier->type != CATTO_AST_NODE_TYPE_EXPRESSION_LEAF) {
        context->errorState = CATTO_ERROR_STATE_UNEXPECTED_TOKEN;
        return;
    }

    char* line = CATTO_NULL;
    bool finished = readLine(&line);

    if (!finished) {
        free(line);
        return;
    }

    catto_TypedValue value = {
        .type = CATTO_DATA_TYPE_STRING,
        .value.asString = line
    };

    catto_setVariable(context, identifier->value.asExpressionLeaf.subjectVariable, value);

    free(line);
}

int main(int argc, char* argv[]) {
    struct termios attributes;
    struct termios originalAttributes;

    tcgetattr(STDIN_FILENO, &attributes);

    originalAttributes = attributes;
    attributes.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &attributes);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    printf("Welcome to catto\n");
    printf("Ready\n");

    catto_Context* context = catto_newContext();

    catto_addContextStandardCommands(context);
    catto_addCommand(context, "input", &inputCommand);

    char* lineString = NULL;

    while (true) {
        size_t size;

        #ifdef DEBUG_MEMORY
            printf("Memory usage: %d\n", mallinfo2().uordblks);
        #endif

        readLine(&lineString);

        if (catto_stringsEqualCaseInsensitive(lineString, "exit")) {
            printf("Goodbye\n");

            tcsetattr(STDIN_FILENO, TCSANOW, &originalAttributes);

            return 0;
        }

        if (catto_stringsEqualCaseInsensitive(lineString, "run")) {
            runCode(context, assembleLines());

            continue;
        }

        if (catto_stringsEqualCaseInsensitive(lineString, "list")) {
            Line* currentLine = firstLine;

            while (currentLine) {
                printf("%s\n", currentLine->code);

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

            if (!firstToken->nextToken) {
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

        runCode(context, catto_copyString(lineString));
    }
}