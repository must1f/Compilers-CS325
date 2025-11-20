// INVALID - Type Error: bool to int narrowing
int main() {
    int x;
    bool y;
    y = true;
    x = y;  // ERROR: bool to int is narrowing
    return x;
}
