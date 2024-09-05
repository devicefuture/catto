#include <stdio.h>
#include <catto-config.h>
#include <catto.h>

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    catto_Context* context = catto_newContext();
    catto_Token* firstToken = catto_tokenise(
        "10 +,-:*,/,^,div,mod,&:|,~\n"
        "20 div mod"
    );

    printf("Tokens: ");
    catto_debugTokens(firstToken);
    printf("\n");

    printf("It works!\n");

    return 0;
}