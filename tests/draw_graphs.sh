# Check if the argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <number_of_test_cases>"
    exit 1
fi

# Save the number of iterations provided as argument
num_iterations=$1

# Timeout for each iteration (30 minutes)
timeout=$((30 * 60))

# Loop from 1 to the specified number of iterations
for ((i=1; i<=num_iterations; i++)); do
  python3 draw_graph.py $i
done