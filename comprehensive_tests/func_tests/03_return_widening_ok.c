// TEST: Return type widening (SHOULD COMPILE)
float get_float() {
    int x;
    x = 42;
    return x;  // int→float widening: OK
}

int main() {
    float f;
    f = get_float();
    return 0;
}
