// TEST: int→bool narrowing in assignment (SHOULD FAIL)
int main() {
    int i;
    bool b;
    i = 5;
    b = i;  // int→bool narrowing: ERROR
    return 0;
}
