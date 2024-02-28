#!/bin/bash

# Check if the argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_iterations>"
    exit 1
fi

# Save the number of iterations provided as argument
num_iterations=$1

# Timeout for each iteration (30 minutes)
timeout=$((30 * 60))

# Loop from 1 to the specified number of iterations
for ((i=10; i<=num_iterations; i++)); do
    echo "Running test $i..."

    # Start time for the iteration
    start_time=$(date +%s)

    # Command to execute for this iteration
    command_to_run="cvc5 --sygus-qe-preproc ./Gtest${i}.sl"

    # Execute the command with timeout
    eval timeout $timeout $command_to_run

    # Check the exit status of the command
    exit_status=$?

    # End time for the iteration
    end_time=$(date +%s)

    # Calculate the time taken for this iteration
    time_taken=$((end_time - start_time))

    # Check if the command execution exceeded the timeout
    if [ $exit_status -eq 124 ]; then
        echo "test $i: Command execution exceeded the timeout of 30 minutes."
    elif [ $exit_status -ne 0 ]; then
        echo "test $i: Error occurred while executing the command."
    else
        echo "test $i: Command executed successfully."
        echo "Time taken: $time_taken seconds"
    fi
done