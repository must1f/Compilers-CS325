// TEST: Use before declaration (SHOULD FAIL)
int main() {
    x = 42;  // ERROR: x not declared yet
    int x;
    return x;
}
