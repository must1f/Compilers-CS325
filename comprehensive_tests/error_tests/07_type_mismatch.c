// TEST: Type mismatch (narrowing) (SHOULD FAIL)
int main() {
    int x;
    float f;

    f = 3.14;
    x = f;  // ERROR: narrowing

    return 0;
}
