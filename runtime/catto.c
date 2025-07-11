#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/time.h>
#include <catto-config.h>
#include <catto.h>

// #define DEBUG_MEMORY

bool inRepl = false;
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
            interrupted = true;

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
        case CATTO_ERROR_STATE_UNKNOWN_PROCEDURE: message = "Procedure is not defined"; break;
        case CATTO_ERROR_STATE_NOT_A_FUNCTION: message = "Attempt to call variable that is not a function"; break;
        case CATTO_ERROR_STATE_NOT_A_LIST: message = "Cannot perform list operation on non-list variable"; break;
        case CATTO_ERROR_STATE_INVALID_LIST_VALUE: message = "Invalid list value"; break;
        case CATTO_ERROR_STATE_CANNOT_ASSIGN_VALUE: message = "Expected variable name"; break;
        case CATTO_ERROR_STATE_UNKNOWN_FIELD: message = "Field is not present in list declaration"; break;

        default: break;
    }

    if (context->errorState != CATTO_ERROR_STATE_NONE) {
        if (context->subjectLineNumber > 0) {
            fprintf(stderr, "%s at line %d\n", message, context->subjectLineNumber);
        } else {
            fprintf(stderr, "%s\n", message);
        }
    }

    free(code);

    if (interrupted) {
        printf("Interrupt\n");
    } else if (inRepl) {
        printf("Ready\n");
    }
}

uint64_t getEpoch() {
    static struct timeval time;

    gettimeofday(&time, NULL);

    return (uint64_t)(time.tv_sec * 1000) + (uint64_t)(time.tv_usec / 1000);
}

void inputCommand(catto_Context* context) {
    catto_AstNode* stringArg = catto_getNextArg(context);
    catto_AstNode* identifierArg = catto_hasNextArg(context) ? catto_getNextArg(context) : CATTO_NULL;

    if (identifierArg) {
        char* string = catto_asString(catto_evalExpression(context, stringArg));

        printf("%s", string);

        free(string);
    } else {
        identifierArg = stringArg;
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

    catto_assignValue(context, identifierArg, value);

    free(line);
}

void delayCommand(catto_Context* context) {
    int64_t delay = catto_asNumber(catto_evalNextArg(context));

    uint64_t startTime = getEpoch();

    if (delay < 0) {
        return;
    }

    while (getEpoch() - startTime < delay) {
        if (getchar() == '\e') { // Escape
            interrupted = true;

            break;
        }
    }
}

void clsCommand(catto_Context* context) {
    printf("\033[2J\033[H");
}

void posCommand(catto_Context* context) {
    int64_t column = catto_asNumber(catto_evalNextArg(context));
    int64_t row = catto_asNumber(catto_evalNextArg(context));

    if (column < 0) {
        column = 0;
    }

    if (row < 0) {
        row = 0;
    }

    printf("\033[%d;%dH", row, column);
}

catto_TypedValue epochFunction(catto_Context* context, catto_DataType returnType) {
    return catto_asTypedNumber(getEpoch());
}

int main(int argc, char* argv[]) {
    struct termios attributes;
    struct termios originalAttributes;

    tcgetattr(STDIN_FILENO, &attributes);

    originalAttributes = attributes;
    attributes.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &attributes);

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    catto_Context* context = catto_newContext();

    catto_addContextStandardCommands(context);
    catto_addCommand(context, "input", &inputCommand);
    catto_addCommand(context, "delay", &delayCommand);
    catto_addCommand(context, "cls", &clsCommand);
    catto_addCommand(context, "pos", &posCommand);
    catto_addFunction(context, "epoch", &epochFunction);

    if (argc >= 2) {
        FILE* fp = fopen(argv[1], "r");

        if (!fp) {
            fprintf(stderr, "Error when reading file\n");

            tcsetattr(STDIN_FILENO, TCSANOW, &originalAttributes);

            return 1;
        }

        fseek(fp, 0, SEEK_END);

        unsigned int size = ftell(fp);
        char* code = (char*)malloc(size + 1);

        fseek(fp, 0, SEEK_SET);

        if (fread(code, sizeof(char), size, fp) != size) {
            fprintf(stderr, "Error reading file contents\n");

            return 1;
        }

        code[size] = '\0';

        runCode(context, code);

        tcsetattr(STDIN_FILENO, TCSANOW, &originalAttributes);

        return context->errorState == CATTO_ERROR_STATE_NONE ? 0 : 1;
    }

    inRepl = true;

    printf("Welcome to catto\n");
    printf("Ready\n");

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

        if (catto_stringsEqualCaseInsensitive(lineString, "new")) {
            Line* currentLine;

            while (currentLine) {
                Line* nextLine = currentLine->nextLine;

                free(currentLine->code);
                free(currentLine);

                currentLine = nextLine;
            }

            firstLine = NULL;

            printf("Created new program\n");

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