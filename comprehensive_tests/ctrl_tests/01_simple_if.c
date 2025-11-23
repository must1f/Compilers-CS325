// TEST: Simple if without else (SHOULD COMPILE)
int main() {
    int x;
    x = 5;

    if (x > 0) {
        x = 10;
    }

    return x;
}
