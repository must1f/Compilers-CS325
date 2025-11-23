// TEST: Duplicate in same scope (SHOULD FAIL)
int main() {
    int x;
    int x;  // ERROR: duplicate in same scope
    return 0;
}
