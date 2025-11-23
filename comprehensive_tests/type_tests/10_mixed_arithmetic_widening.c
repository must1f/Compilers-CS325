// TEST: Mixed int/float arithmetic (SHOULD COMPILE)
int main() {
    int i;
    float f;
    float result;

    i = 5;
    f = 3.14;
    result = i + f;  // int→float widening for arithmetic: OK

    return 0;
}
