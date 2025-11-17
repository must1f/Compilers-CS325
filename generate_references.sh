#!/bin/bash

echo "Generating reference LLVM IR using Clang..."

# For each test with .c file
for test_dir in tests/*/; do
    test_name=$(basename "$test_dir")
    
    # Skip array tests for now
    if [[ "$test_name" == array* ]] || [[ "$test_name" == *array* ]]; then
        continue
    fi
    
    # Find .c file
    c_file=$(find "$test_dir" -name "*.c" | head -1)
    
    if [ -n "$c_file" ]; then
        echo "Processing: $test_name"
        
        # Generate IR with clang
        clang -S -emit-llvm -O0 "$c_file" -o "${test_dir}reference.ll" 2>/dev/null
        
        if [ $? -eq 0 ]; then
            echo "  ✓ Generated reference.ll"
        else
            echo "  ✗ Failed"
        fi
    fi
done

echo "Done!"