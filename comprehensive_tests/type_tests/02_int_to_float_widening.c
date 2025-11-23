// TEST: int→float widening (SHOULD COMPILE)
int main() {
    int i;
    float f;
    i = 42;
    f = i;  // int→float widening: ALLOWED
    return 0;
}
