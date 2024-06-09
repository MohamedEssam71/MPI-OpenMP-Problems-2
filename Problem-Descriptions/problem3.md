# Problem 3: Calculating Standard Deviation using OpenMP

## Problem Statement

Write a parallel C program to calculate the standard deviation of randomly generated elements.

### How to Calculate Standard Deviation:

1. Calculate the Mean (sum of elements / number of elements)
2. For each number, subtract the mean and square the result ( (element - mean)^2 )
3. Calculate the mean of the squared differences
4. Take the square root of step 3

### Parallelization Scenario

- Get `n` from user and make it shared.
- Generate `n` random elements in parallel.
- Calculate local sum and local mean of the generated `n` elements.
- Calculate local sum of squared differences from the mean.
- Add local sum of squared differences to global sum.
- Calculate the global mean and standard deviation.

### Example Input and Output

**Input:**

```
n = 5
```

**Output:**

```
Standard deviation of 20 elements: 2.87
```
