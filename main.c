#include "./transpiler/transpiler.h"

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        transpiler("C:\\Users\\user\\Link\\_example\\main.link");
    }
    else
    {
        transpiler(argv[1]);
    }

    return 0;
}