#include <stdio.h>
#include <catto-config.h>
#include <catto.h>

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    catto_Context* context = catto_newContext();
    catto_Token* firstToken = catto_tokenise(
        "10 +,-:*,/,^,div,mod,&:|,~\n"
        "20 print \"this is cool\", 123, 5 * 10.5\n"
        "30 print 2.5e-6 div y$ mod z"
    );

    printf("Tokens: ");
    catto_debugTokens(firstToken);
    printf("\n");

    firstToken = catto_tokenise(
        "10 print\n"
        "20 print 123\n"
        "30 print \"hello\", 456 + 789 * 12, \"world\"\n"
        "40 print 10 - 9 - 8 - 7 - 6\n"
        "50 print 6 * (4 + 5)\n"
        "60 print -5, 6 + 5 * -3, -(2 + 4)\n"
        "60 print var, var[5], var[2 + 3]\n"
    );

    printf("Tokens: ");
    catto_debugTokens(firstToken);
    printf("\n");

    catto_AstNode* firstAstNode = catto_parse(firstToken);

    printf("AST nodes: ");
    catto_debugAstNodes(firstAstNode);
    printf("\n");

    printf("It works!\n");

    return 0;
}