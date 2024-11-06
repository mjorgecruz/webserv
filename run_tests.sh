#!/bin/bash

# Initialize counters
total_tests=0
passed_tests=0

# Function to run tests on a given directory with an expected exit code
run_tests() {
    local dir=$1
    local expected_exit_code=$2

    for file in "$dir"/*.conf; do
        ((total_tests++))

        # Run the webserver with the test file, silencing its output
        ./webserv "$file" > /dev/null 2>&1
        
        # Check the exit status
        exit_code=$?
        if [ $exit_code -eq $expected_exit_code ]; then
            # Green for passed tests
            echo -e "\e[1;32m✅ $(basename "$file")\e[0m"
            ((passed_tests++))
        else
            # Red for failed tests
            echo -e "\e[1;31m❌ $(basename "$file")\e[0m"
        fi
    done
}

# Run tests for files in testconfs folder (should return 255 to pass)
run_tests "testconfs" 255

# Run tests for files in testconfs/works folder (should return 1 to pass)
run_tests "testconfs/works" 1

# Print summary
echo -e "\n========================"
echo -e "         Summary        "
echo -e "========================"
echo -e "Passed: $passed_tests/$total_tests"
echo -e "========================"

# Check if all tests passed or if there were any failures
if [ $passed_tests -eq $total_tests ]; then
    # Green success message
    echo -e "\e[1;32m🎉🎉🎉 ALL TESTS PASSED! 🎉🎉🎉\e[0m"
else
    # Red failure message
    echo -e "\e[1;31m💀💀💀 YOU FAILED! 💀💀💀\e[0m"
    echo -e "\e[1;31mTry again! Don't let the tests win!\e[0m"
fi
