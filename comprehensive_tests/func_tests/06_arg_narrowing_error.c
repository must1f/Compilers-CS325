// TEST: Argument type narrowing (SHOULD FAIL)
void takes_int(int i) {
    return;
}

int main() {
    float f;
    f = 3.14;
    takes_int(f);  // float→int narrowing: ERROR
    return 0;
}
