// INVALID - Type Error: unary minus requires numeric type
int main() {
    bool x;
    bool y;
    x = true;
    y = -x;  // ERROR: cannot negate boolean
    return 0;
}
