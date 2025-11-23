// TEST: Nested if statements (SHOULD COMPILE)
int main() {
    int x;
    int y;
    int result;

    x = 5;
    y = 10;
    result = 0;

    if (x > 0) {
        if (y > 0) {
            result = 1;
        } else {
            result = 2;
        }
    } else {
        result = 3;
    }

    return result;
}
