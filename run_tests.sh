#!/bin/bash

total_tests=0
passed_tests=0
failed_files=()

print_header() {
    local header_text=$1
    echo -e "\n\e[1;33m========== $header_text ==========\e[0m"
}

run_tests() {
    local dir=$1
    local expected_exit_code=$2

    for file in "$dir"/*.conf; do
        ((total_tests++))

        ./webserv "$file" > /dev/null 2>&1
        
        exit_code=$?
        if [ $exit_code -eq $expected_exit_code ]; then
            echo -e "\e[1;32m✅ $(basename "$file")\e[0m"
            ((passed_tests++))
        else
            echo -e "\e[1;31m❌ $(basename "$file")\e[0m"
            failed_files+=("$(basename "$file")")
        fi
    done
}

print_header "LOCATION SIDE CONFIG ERRORS"
run_tests "/home/luis-ffe/webserv/testconfs/ko_location" 255

print_header "SERVER SIDE CONFIG ERRORS"
run_tests "/home/luis-ffe/webserv/testconfs/ko_server" 255

print_header "WORKING CONFIGURATIONS"
run_tests "/home/luis-ffe/webserv/testconfs/ok_working" 1

echo -e "\n========================"
echo -e "         Summary        "
echo -e "========================"
echo -e "Passed: $passed_tests/$total_tests"
echo -e "========================"

if [ ${#failed_files[@]} -gt 0 ]; then
    echo -e "\n\e[1;33m========== NOT PASSED IN: ==========\e[0m"
    echo -e
    for failed_file in "${failed_files[@]}"; do
        echo -e "\e[1;31m❌ $failed_file\e[0m"
    done
fi

if [ $passed_tests -eq $total_tests ]; then
    echo -e
    echo -e "\e[1;32m🎉🎉🎉 ALL TESTS PASSED! 🎉🎉🎉\e[0m"
else
    echo -e
    echo -e "\e[1;31m💀💀💀 YOU FAILED! 💀💀💀\e[0m"
    echo -e "\e[1;31mTry again! Don't let the tests win!\e[0m"
fi
