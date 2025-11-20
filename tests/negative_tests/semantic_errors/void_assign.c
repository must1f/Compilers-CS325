// INVALID - Type Error: void has no value
// Expected: "cannot use void expression"
extern void print_int(int x);

int main() {
    int x;
    x = print_int(5);  // ERROR: print_int returns void
    return x;
}
