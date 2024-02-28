

# write a code for running a given test number given file 1 and file 2 



#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <test_number> <string1> <string3>"
    exit 1
fi

make clean
make run

# Extract command line arguments
test_number="$1"
string1="$2"
string3="$3"

# Variable to store the return value
../exec/test"${test_number}" "${string1}" "${string3}" "0" "1" 
echo "TC1.1 DONE"
../exec/test"${test_number}" "${string1}" "${string3}" "2" "2"
echo "TC1.2 DONE"
../exec/test"${test_number}" "${string1}" "${string3}" "2" "3"
echo "TC1.3 DONE"
../exec/test"${test_number}" "${string1}" "${string3}" "2" "4"
echo "TC1.4 DONE"
../exec/test"${test_number}" "${string1}" "${string3}" "2" "5"
echo "TC1.5 DONE"

#     # Store the return value
# return_value=$?

# # Run the executable until the return value is 111
# while [ "$return_value" -ne 111 ]; do
#     # Run the executable with the provided arguments
#     ../exec/test"${test_number}" "${string1}" "${string3}" "1"

#     # Store the return value
#     return_value=$?

#     # Print the return value for reference
#     echo "Return value: $return_value"

#     # Check if the return value is 111; if not, continue the loop
#     if [ "$return_value" -ne 111 ]; then
#         echo "Re-running with new arguments..."
#     fi
# done

echo "Script completed."
