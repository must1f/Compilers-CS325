// VALID - argument widening is allowed
// Should have: sitofp for int->float conversion
float foo(float x) {
    return x * 2.0;
}

int main() {
    int y;
    float result;
    y = 3;
    result = foo(y);  // OK: int to float is widening
    return 0;
}
