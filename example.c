#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int multiply(int a, int b) {
    return a * b + 5;
}

int func(int a, int b) {
    return multiply(a, b);
}

char* concat(char *x, char *y) {
    int size = strlen(x) + strlen(y) + 1;
    char *new = (char *)malloc(size);
    strcpy(new, x);
    strcat(new, y);

    return new;
}

void sum(int a, int b) {
    int c = a + b;
    int d = c * a;
}

int main(int argc, char **argv) {
    int a = 2, b = 3;
    int c = multiply(a, b);

    char *x = "Hello";
    char *y = " World";
    char *z = concat(x, y);
    printf("%s\n", z);
}
