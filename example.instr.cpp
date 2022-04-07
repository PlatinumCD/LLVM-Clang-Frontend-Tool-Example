#include <cstdio>
#include <string>

using namespace std;

/*
	multiply
		Parameters:
			a (int)
			b (int)
*/
int multiply(int a, int b) {
    return a * b;
}

/*
	concat
		Parameters:
			x (std::string)
			y (std::string)
*/
string concat(string x, string y) {
    return x + y;
}

/*
	main
*/
int main() {
    int a = 2, b = 3;
    int c = multiply(a, b);

    string x = "cameron";
    string y = " durbin";
    string z = concat(x, y);

    printf("%d\n", c);
    printf("%s\n", z.c_str());
}
