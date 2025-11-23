// TEST: bool→int widening (SHOULD COMPILE)
int main() {
    bool b;
    int i;
    b = true;
    i = b;  // bool→int widening: ALLOWED
    return i;
}
