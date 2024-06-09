# Problem 1: Election to Choose a New President (using MPI)

## Problem Statement

Write a parallel C program to simulate an election process to choose a new president.

### Election Rules:

1. There are C candidates (numbered from 1 to C) and V voters.
2. The election process consists of up to 2 rounds. All candidates compete in the first round. If a candidate receives more than 50% of the votes, he wins. Otherwise, another round takes place, where only the top 2 candidates compete. The candidate who receives more votes than his opponent wins and becomes the new president.
3. Voters' preferences are the same in both rounds, and each voter must vote exactly once in each round for a currently competing candidate according to his preferences.

### Program Requirements

- Generate a file containing voters' preferences. The format of the file must be:
  - Number of candidates in the first line
  - Number of voters in the second line
  - Voters' preferences in the following lines
- When running the program, prompt the user to generate the file or calculate the result. If the user chooses to calculate the result, enter the filename as input.
- Each process should load its part of the file.
- Print to the console the steps happening in every process and which candidate will win the elections and in which round.
- If there are 2 rounds, identify that and show the percentage of every candidate per each round.

### Example Input and Output

**Input:**

```
3 5
1 2 3
1 2 3
2 1 3
2 3 1
3 2 1
```

**Output:**

```
2 2 // candidate 2 wins in round 2
```
