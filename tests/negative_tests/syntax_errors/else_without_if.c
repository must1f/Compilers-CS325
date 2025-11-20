// INVALID - Syntax Error: else must follow if
int main() {
    int x;
    else {  // ERROR: else without if
        x = 5;
    }
    return x;
}
