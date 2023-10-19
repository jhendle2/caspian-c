
int doAThing() { return 5; }
int doAThing2(int x) { return x+y; }

int func() {
    int declaration;
    int definition = 5 - 2;
    char c = 'x';
    float f = -1.234;
    static const char* s = "Hello";
    int x = doAThing(doAThing2(5, 9, 2)) + 5;
    return 5;
}