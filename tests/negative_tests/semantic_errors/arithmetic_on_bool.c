// INVALID - Type Error: arithmetic requires numeric types
int main() {
    bool x;
    bool y;
    int result;
    x = true;
    y = false;
    result = x + y;  // ERROR: cannot add booleans
    return result;
}
