// INVALID - Scope Error: global array not declared
// Expected: "undefined variable 'global_arr'"
int main() {
    int x;
    x = global_arr[0];  // ERROR: global_arr not declared
    return x;
}
