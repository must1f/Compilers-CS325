#!/bin/bash
# save as run_lexer_tests.sh

OUTPUT_DIR="lexer_outputs"
mkdir -p $OUTPUT_DIR

# Loop through all test directories
for test_dir in tests/*/; do
    test_name=$(basename $test_dir)
    c_file="${test_dir}${test_name}.c"
    
    if [ -f "$c_file" ]; then
        echo "Testing: $test_name"
        ./mccomp "$c_file" > "${OUTPUT_DIR}/${test_name}_tokens.txt" 2>&1
        echo "Output saved to ${OUTPUT_DIR}/${test_name}_tokens.txt"
    fi
done

echo "All tests complete. Results in $OUTPUT_DIR/"