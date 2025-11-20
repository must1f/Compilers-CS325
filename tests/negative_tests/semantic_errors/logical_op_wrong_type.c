// INVALID - Type Error: logical operators need bool operands
int main() {
    int x;
    int y;
    bool result;
    x = 5;
    y = 10;
    result = x && y;  // ERROR: && requires bool operands (int to bool is narrowing)
    return 0;
}
