// TEST: All comparison operators (SHOULD COMPILE)
int main() {
    int x;
    int y;
    bool result;

    x = 5;
    y = 10;

    result = x < y;
    result = x <= y;
    result = x > y;
    result = x >= y;
    result = x == y;
    result = x != y;

    return 0;
}
