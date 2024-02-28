#!/bin/bash

# Check if the argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_tests>"
    exit 1
fi

# Save the number of tests provided as argument
num_tests=$1

echo "Current time: $(date +"%T")" > ../logs/console.log
# Loop through the tests
for ((i=14; i<=num_tests; i++)); do
    # Change the TESTFILE in the makefile to test<i>.cpp
    sed -i "s/TESTFILE =.*/TESTFILE = test${i}.cpp/" makefile
    
    # Run make
    make run

    echo "Started test${i}" >> ../logs/console.log

    ../exec/test${i} dump1 dump2 0 1
    
    # Check if make was successful
    if [ $? -ne 111 ]; then
        echo "Error occurred while compiling test${i}.cpp"
        exit 1
    fi
    # Get the first line
    echo "Succesful test${i}=============================================\n" >> ../logs/console.log

    head -n 1 ../logs/output_tc${i}_prog_sys_parallel10-1.log > input.txt.tmp

    # Get the last three lines
    tail -n 3 ../logs/output_tc${i}_prog_sys_parallel10-1.log >> input.txt.tmp
    
    cat input.txt.tmp >> ../logs/console.log
    echo "\n-------------------------------------------------------\n" >> ../logs/console.log
    # Replace the original file with the temporary file
    mv input.txt.tmp ../logs/output_tc${i}_prog_sys_parallel10-1.log


    

    echo "Succesful test${i}" >> ../logs/console.log


   
done