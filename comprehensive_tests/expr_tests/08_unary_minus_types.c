// TEST: Unary minus on numeric types (SHOULD COMPILE)
int main() {
    int i;
    float f;

    i = -42;
    f = -3.14;

    i = -i;
    f = -f;

    return 0;
}
