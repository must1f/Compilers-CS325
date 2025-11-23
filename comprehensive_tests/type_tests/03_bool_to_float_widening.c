// TEST: bool→float widening (SHOULD COMPILE)
int main() {
    bool b;
    float f;
    b = true;
    f = b;  // bool→float widening (via int): ALLOWED
    return 0;
}
