// TEST: Argument type widening (SHOULD COMPILE)
void takes_float(float f) {
    return;
}

int main() {
    int x;
    x = 42;
    takes_float(x);  // int→float widening: OK
    return 0;
}
