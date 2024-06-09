#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


const int MAX = 100;

int main() {
    int n, threads;
    printf("Enter number of threads: ");
    scanf("%d", &threads);
    fflush(stdout);

    omp_set_num_threads(threads);


    printf("Enter n: ");
    scanf("%d", &n);
    fflush(stdout);

    srand(time(NULL));

    double globalSum = 0;

#pragma omp parallel shared(n, globalSum)
    {

        int *arr = malloc(n * sizeof(int));
        int sum = 0;
        for (int i = 0; i < n; i++) {
            arr[i] = rand() % MAX + 1;
            sum += arr[i];
        }
        double localMean = 1.0 * sum / n;
        double localSumOSqD = 0;
        for (int i = 0; i < n; i++) {
            localSumOSqD += (arr[i] - localMean) * (arr[i] - localMean);
        }
        printf("Thread %d localSumOSqD: %f\n", omp_get_thread_num(), localSumOSqD);
        fflush(stdout);

        free(arr);

#pragma omp critical
        globalSum += localSumOSqD;
    }

    double globalMean = globalSum / (n * threads);
    double deviation = sqrt(globalMean);
    printf("Standard deviation: %f\n", deviation);
    fflush(stdout);
}