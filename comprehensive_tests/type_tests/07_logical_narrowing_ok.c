// TEST: Logical operators allow narrowing (SHOULD COMPILE)
int main() {
    int x;
    int y;
    bool result;

    x = 5;
    y = 10;

    result = x && y;  // int→bool for &&: OK
    result = x || y;  // int→bool for ||: OK
    result = !x;      // int→bool for !: OK

    return 0;
}
