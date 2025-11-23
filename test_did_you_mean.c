// Test for "Did you mean?" functionality
// This file intentionally contains typos to test suggestions

int main() {
    int counter;
    int value;
    int result;
    float temperature;

    counter = 10;
    value = 5;
    temperature = 98.6;

    // Typo: should be 'counter' not 'countr'
    result = countr + value;

    // Typo: should be 'temperature' not 'temperatur'
    return temperatur;
}
