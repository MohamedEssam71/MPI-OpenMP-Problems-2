#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mpi.h"

int countDigits(int number) {
    int count = 0;
    while (number > 0) {
        number /= 10;
        count++;
    }
    return count;
}

void generatePermutation(int *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = i + 1;
    }
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void promptForFileGeneration() {
    int candidates, voters;
    printf("Enter the number of candidates and number of voters:\n");
    fflush(stdout);
    scanf("%d %d", &candidates, &voters);
    


    FILE *file = fopen("test.txt", "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "%d %d\n", candidates, voters);

    int *permutation = malloc(candidates * sizeof(int));
    if (permutation == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return;
    }

    for (int i = 0; i < voters; i++) {
        generatePermutation(permutation, candidates);
        for (int j = 0; j < candidates; j++) {
            fprintf(file, (j != candidates - 1 ? "%d " : "%d"), permutation[j]);
        }
        fprintf(file, "\n");
    }

    free(permutation);
    fclose(file);
    printf("File 'test.txt' has been created with random permutations.\n");
    fflush(stdout);
}

int getNumberOfBytesInLine(int candidates) {
    int totalBytes = 0;
    int spaces = candidates - 1;
    int newline = 1;

    for (int i = 1; i <= candidates; i++) {
        totalBytes += countDigits(i);
    }

    totalBytes += spaces + newline;
    return totalBytes;
}

char **readVoterPreferences(char *fileName, int rank, int offset, int portion) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return NULL;
    }

    int candidates, voters;
    fscanf(file, "%d %d", &candidates, &voters);
    fflush(stdout);
    fseek(file, countDigits(candidates) + countDigits(voters) + 2,
          SEEK_SET);

    int linesToSkip = rank * offset;
    int lineSize = getNumberOfBytesInLine(candidates);
    fseek(file, linesToSkip * lineSize, SEEK_CUR);

    char **preferences = malloc((portion + 1) * sizeof(char *));
    for (int i = 0; i < portion; i++) {
        preferences[i] = malloc((lineSize + 1) * sizeof(char));
        fgets(preferences[i], lineSize + 1, file);
    }
    fclose(file);
    return preferences;
}

void freePreferences(char **preferences, int portion) {
    for (int i = 0; i < portion; i++) {
        free(preferences[i]);
    }
    free(preferences);
}

int *computeVotes(char **preferences, int rank, int portion, int candidates) {
    int *votes = calloc(candidates + 1, sizeof(int));

    char *saveptr;
    for (int i = 0; i < portion; i++) {
        int n = strlen(preferences[i]);
        char *tmp = malloc(n * sizeof(char));
        for (int j = 0; j < n; j++) {
            tmp[j] = preferences[i][j];
        }
        char *token = strtok_r(tmp, " ", &saveptr);
        if (token != NULL) {
            int candidate = atoi(token);
            votes[candidate]++;
        }
    }

    for (int i = 1; i <= candidates; ++i) {
        printf("Candidate %d received %d votes from process %d\n", i, votes[i], rank);
        fflush(stdout);
    }
    return votes;
}

int *computeVotesAgain(char **preferences, int rank, int portion, int firstCandidate, int secondCandidate) {
    int *votes = calloc(2, sizeof(int));
    char *saveptr;

    for (int i = 0; i < portion; i++) {
        char *token = strtok_r(preferences[i], " ", &saveptr);
        while (token != NULL) {
            int candidate = atoi(token);
            if (candidate == firstCandidate) {
                votes[0]++;
                break;
            } else if (candidate == secondCandidate) {
                votes[1]++;
                break;
            }
            token = strtok_r(NULL, " ", &saveptr);
        }
    }

    printf("Candidate %d received %d votes from process %d\n", firstCandidate, votes[0], rank);
    fflush(stdout);
    printf("Candidate %d received %d votes from process %d\n", secondCandidate, votes[1], rank);
    fflush(stdout);

    return votes;
}

void displayResults(int *votes, int candidates, int voters) {
    for (int i = 1; i <= candidates; i++) {
        printf("Candidate %d received %d votes (%.2f%%)\n", i, votes[i], (100.0 * votes[i]) / voters);
        fflush(stdout);
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char fileName[100] = "test.txt";
    int candidates = 0, voters = 0;

    if (rank == 0) {
        printf("1: Generate voter preferences file\n2: Calculate election results\n");
        fflush(stdout);
        int choice;
        scanf("%d", &choice);
        while (choice == 1) {
            promptForFileGeneration();
            printf("1: Generate voter preferences file\n2: Calculate election results\n");
            fflush(stdout);
            scanf("%d", &choice);
        }
        printf("Enter the file name:\n");
        fflush(stdout);
        scanf("%s", fileName);
        FILE *file = fopen(fileName, "r");
        fscanf(file, "%d %d", &candidates, &voters);
        fclose(file);

    }

    MPI_Bcast(&candidates, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&voters, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int portion = voters / size;
    int offset = portion;
    if (rank == size - 1) {
        portion += voters % size;
    }

    char **preferences = readVoterPreferences(fileName, rank, offset, portion);
    int *localVotes = computeVotes(preferences, rank, portion, candidates);
    int *globalVotes = calloc(candidates + 1, sizeof(int));

    MPI_Reduce(localVotes, globalVotes, candidates + 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    int firstCandidate = 0, secondCandidate = 0;

    int flag = 0;

    if (rank == 0) {
        printf("\nRound 1 results:\n");
        fflush(stdout);
        displayResults(globalVotes, candidates, voters);
        for (int i = 1; i <= candidates; ++i) {
            if (globalVotes[i] >= globalVotes[firstCandidate]) {
                secondCandidate = firstCandidate;
                firstCandidate = i;
            } else if (globalVotes[i] > secondCandidate) {
                secondCandidate = i;
            }
        }
        if (globalVotes[firstCandidate] * 2 > voters) {
            printf("Candidate %d won in round 1 with (%.2f%%)\n", firstCandidate,
                   globalVotes[firstCandidate] * 100.0 / voters);
                   fflush(stdout);
            free(globalVotes);
            flag = 1;

        } else {
            printf("\nRound 2 start:\n");
            fflush(stdout);
            free(globalVotes);
        }
    }

    MPI_Bcast(&flag, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (flag == 1) {
        goto GO;
    }

    MPI_Bcast(&firstCandidate, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&secondCandidate, 1, MPI_INT, 0, MPI_COMM_WORLD);

    localVotes = computeVotesAgain(preferences, rank, portion, firstCandidate, secondCandidate);
    globalVotes = calloc(2, sizeof(int));
    MPI_Reduce(localVotes, globalVotes, 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\nRound 2 results:\n");
        fflush(stdout);
        if (globalVotes[0] > globalVotes[1]) {
            printf("Candidate %d won in round 2 with (%.2f%%)\n", firstCandidate, globalVotes[0] * 100.0 / voters);
            fflush(stdout);
        } else if (globalVotes[1] > globalVotes[0]) {
            printf("Candidate %d won in round 2 with (%.2f%%)\n", secondCandidate, globalVotes[1] * 100.0 / voters);
            fflush(stdout);
        } else {
            printf("It is a tie between Candidates %d and %d!\n", firstCandidate, secondCandidate);
            fflush(stdout);
        }
        free(globalVotes);
    }
    GO:
    free(localVotes);
    freePreferences(preferences, portion);
    MPI_Finalize();
    return 0;
}