// TEST: Unary operator chains (SHOULD COMPILE)
int main() {
    int x;
    bool b;

    x = 5;
    x = -(-x);    // Double negation

    b = true;
    b = !!b;      // Double logical NOT

    return 0;
}
