// TEST: Empty blocks (SHOULD COMPILE)
int main() {
    int x;
    x = 5;

    if (x > 0) {
    }

    while (x < 10) {
        x = x + 1;
    }

    {
    }

    return x;
}
