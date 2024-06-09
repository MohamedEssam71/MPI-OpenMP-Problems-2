#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

const int MAX = 10;

int **generateMatrix(int rows, int columns)
{
    int **matrix = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (int *)malloc(columns * sizeof(int));
    }

#pragma omp for collapse(2)
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            matrix[i][j] = rand() % MAX + 1;
        }
    }

    return matrix;
}

int *searchForKey(int rows, int columns, int key, int **matrix, int *index)
{
    int *answerArray = malloc(rows * columns * sizeof(int));
#pragma omp parallel shared(rows, columns, key, matrix, index)
    {
#pragma omp for collapse(2)
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                if (matrix[i][j] == key)
                {
                    printf("thread %d found a key at index (%d, %d)\n", omp_get_thread_num(), i, j);
#pragma omp critical
                    answerArray[(*index)++] = i * columns + j;
                }
            }
        }
    }
    return answerArray;
}

int main()
{
    int rows, columns, key, numOfThreads;

    printf("Enter number of threads: ");
    scanf("%d", &numOfThreads);
    fflush(stdout);

    omp_set_num_threads(numOfThreads);

    printf("Enter number of rows: ");
    scanf("%d", &rows);
    fflush(stdout);

    printf("Enter number of columns: ");
    scanf("%d", &columns);
    fflush(stdout);

    printf("Enter key: ");
    scanf("%d", &key);
    fflush(stdout);

    srand(time(NULL));

    int **matrix = generateMatrix(rows, columns);

    int *index = malloc(sizeof(int));
    *index = 0;
    int *answerArray = searchForKey(rows, columns, key, matrix, index);

    if (*index == 0)
    {
        printf("The key wasn't found at any cell: -1\n");
    }
    else
    {
        printf("Found the key at cells: ");
        for (int i = 0; i < *index; ++i)
        {
            printf("(%d, %d) ", answerArray[i] / columns, answerArray[i] % columns);
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }

    free(matrix);
    free(answerArray);
    free(index);
}