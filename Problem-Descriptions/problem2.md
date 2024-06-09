# Problem 2: Search in Array using OpenMP

## Problem Statement

Write a C program to search for a key in a 2D array using OpenMP.

### Program Requirements

- Initialize the matrix with random numbers.
- Share the matrix with all processes.
- Return the indices of the key if found, otherwise return -1.

### Parallelization Scenario

- Use `#pragma omp parallel for` directive to divide the search loop iterations among processes.
- Every time the key is found, display which process found it at what index and add that index to an array.
- Print the array of indices where the key was found.

### Example Input and Output

**Input:**

```
row = 5, column = 5, key = 7
```

**Output:**

```
Process 1: Found key at index (1, 3)
Process 2: Found key at index (4, 2)
Indices of key: [(1, 3), (4, 2)]
```
