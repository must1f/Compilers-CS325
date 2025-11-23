// TEST: float→int narrowing (SHOULD FAIL)
int main() {
    float f;
    int i;
    f = 3.14;
    i = f;  // float→int narrowing: ERROR
    return 0;
}
