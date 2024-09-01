#include <stdio.h>
#include <catto-config.h>
#include <catto.h>

int main(int argc, char* argv[]) {
    printf("Hello, world!\n");

    catto_Context* context = catto_newContext();

    printf("It works!\n");

    return 0;
}