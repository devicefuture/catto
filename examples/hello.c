#include <stdio.h>
#include <stdbool.h>
#include <catto-config.h>
#include <catto.h>

// #define TEST_MEMORY

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    catto_Context* context = catto_newContext();

    catto_addContextStandardCommands(context);

    catto_Token* firstToken = catto_tokenise(
        context,
        "10 +,-:*,/,^,div,mod,&:|,~\n"
        "20 print \"this is cool\", 123, 5 * 10.5\n"
        "30 print 2.5e-6 div y$ mod z"
    );

    printf("Tokens: ");
    catto_debugTokens(firstToken);
    printf("\n");

    firstToken = catto_tokenise(
        context,
        "10 print\n"
        "20 print 123\n"
        "30 print \"hello\", 456 + 789 * 12, \"world\"\n"
        "40 print 10 - 9 - 8 - 7 - 6\n"
        "50 print 6 * (4 + 5)\n"
        "60 print -5, 6 + 5 * -3, -(2 + 4)\n"
        "70 print abs(-3.5), upper(\"Testing\"), max(1, 3)\n"
        "80 print var, var[5], var[2 + 3]"
    );

    printf("Tokens: ");
    catto_debugTokens(firstToken);
    printf("\n");

    catto_AstNode* firstAstNode = catto_parse(firstToken);

    printf("AST nodes: ");
    catto_debugAstNodes(firstAstNode);
    printf("\n");

    catto_load(
        context,
        "10 print \"Hello, world!\"\n"
        "20 print 1, 2 + 4 - 1 + 4 - 7, -3, 4.59e-8, (3.7 * (4.6 + 9.1))^3, 1 + 2 * 3 = 7 and 5 > 3\n"
        "70 print round(-3.5), floor(-3.5), ceil(-3.5), abs(-3.5)\n"
        "80 print upper(\"Testing!\"), max(1, 3)"
    );

    catto_run(context);

    printf("It works!\n");

    #ifdef TEST_MEMORY
        while (true) {
            catto_load(
                context,
                "10 print \"Hello, world!\"\n"
                "20 x=x+1"
                "30 print \"x is:\", x\n"
                "40 print \"concat \"; x\n"
                "50 y=x; \" concat\"\n"
                "60 print y$\n"
                "70 print max(1, 3)\n"
                "80 print upper(\"Hello, world!\")"
            );

            catto_run(context);
        }
    #endif

    return 0;
}