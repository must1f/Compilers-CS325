// TEST: Modulo requires integers (SHOULD FAIL)
int main() {
    float x;
    float y;
    float result;

    x = 5.5;
    y = 2.2;
    result = x % y;  // Modulo on float: ERROR

    return 0;
}
