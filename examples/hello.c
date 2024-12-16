#include <stdio.h>
#include <stdbool.h>
#include <catto-config.h>
#include <catto.h>

#define TEST_MEMORY

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
        "60 print var, var[5], var[2 + 3]"
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
        "20 print 1, 2 + 4 - 1 + 4 - 7, -3, 4.59e-8, (3.7 * (4.6 + 9.1))^3, 1 + 2 * 3 = 7 and 5 > 3"
    );

    catto_run(context);

    printf("It works!\n");

    while (true) {
        catto_load(
            context,
            "10 print \"Hello, world!\"\n"
            "20 x=x+1\n"
            "30 print \"x is:\", x\n"
        );

        catto_run(context);
    }

    return 0;
}