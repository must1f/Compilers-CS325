// TEST: Variable out of scope (SHOULD FAIL)
int main() {
    {
        int x;
        x = 42;
    }
    return x;  // ERROR: x out of scope
}
