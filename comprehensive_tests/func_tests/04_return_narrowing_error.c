// TEST: Return type narrowing (SHOULD FAIL)
int get_int() {
    float f;
    f = 3.14;
    return f;  // float→int narrowing: ERROR
}

int main() {
    return get_int();
}
