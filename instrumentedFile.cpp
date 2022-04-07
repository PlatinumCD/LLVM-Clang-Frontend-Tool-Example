#include <cstdio>
#include <string>

using namespace std;

// Start function multiply
int multiply(int a, int b) {
    return a * b;
}

// Start function concat
string concat(string x, string y) {
    return x + y;
}

// Start function main
int main() {
    int a = 2, b = 3;
    int c = multiply(a, b);

    string x = "cameron";
    string y = " durbin";
    string z = concat(x, y);

    printf("%d\n", c);
    printf("%s\n", z.c_str());
}
