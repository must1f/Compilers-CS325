// INVALID - Scope Error: variable used outside its scope
// Expected: "undefined variable 'x'"
int main() {
    int result;
    {
        int x;
        x = 5;
    }
    result = x;  // ERROR: x is out of scope (if MiniC supports nested blocks)
    return result;
}
