// TEST: Local shadowing global (SHOULD COMPILE)
int x;

int main() {
    int x;  // Shadows global: OK
    x = 42;
    return x;
}
