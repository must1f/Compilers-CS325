// INVALID - Type Error: comparison returns bool, assigning to int is narrowing
int main() {
    int x;
    x = (5 > 3);  // ERROR: bool to int narrowing
    return x;
}
