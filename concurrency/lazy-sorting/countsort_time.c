#include "main.h"
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


typedef struct {
    type* array;
    int start;
    int end;
    int min_time;
    int* count;
    pthread_mutex_t* mutex;
} ThreadArgs;

void* countsort_timethread(void* args) {
    // printf("countsort_thread\n");
    ThreadArgs* targs = (ThreadArgs*)args;
    type* a = targs->array;
    int start = targs->start;
    int end = targs->end;
    int min_time = targs->min_time;
    int* count = targs->count;
    pthread_mutex_t* mutex = targs->mutex;

    // Update the count array
    for (int i = start; i < end; i++) {
        int index = a[i].timestamp - min_time;
        pthread_mutex_lock(&mutex[index]);
        count[index]++;
        pthread_mutex_unlock(&mutex[index]);
    }

    pthread_exit(NULL);
}

void countsort_timestamp(void* args) {
    // printf("countsort_timestamp\n");
    int n = ((SortArgs*)args)->size;
    printf("%d\n", n);
    type* array = ((SortArgs*)args)->array;
    // printf("countsort_id\n");
    pthread_t threads[nthreads];
    ThreadArgs targs[nthreads];
    // Find the maximum and minimum ID values
    int max_time = array[0].timestamp;
    int min_time = array[0].timestamp;
    for (int i = 0; i < n; i++) {
        if (array[i].timestamp > max_time) {
            max_time = array[i].timestamp;
        }
        if (array[i].timestamp < min_time) {
            min_time = array[i].timestamp;
        }
    }
    // printf("max_id: %d\n", max_time);
    // printf("min_id: %d\n", min_time);

    // Create and initialize the count array
    int range = max_time - min_time + 1;
    // printf("range: %d\n", range);
    pthread_mutex_t* mutex = (pthread_mutex_t*)malloc(range * sizeof(pthread_mutex_t));
    int* count = (int*)calloc(range, sizeof(int));
    if (count == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Initialize mutexes
    for (int i = 0; i < range; i++) {
        pthread_mutex_init(&mutex[i], NULL);
    }

    // Create threads to update the count array
    
    // printf("chunk_size: %d\n", chunk_size);
    // threads needed
    int t = (n%chunk_size==0)?(int)(n/chunk_size) : (n/chunk_size) + 1;
    // printf("threads needed %d\n", t);
    for (int i = 0; i < t; i++) {
        printf("%d\n", i);
        targs[i].array = array;
        targs[i].start = i * chunk_size;
        targs[i].end = (i == t-1) ? n : (i + 1) * chunk_size;
        // printf("start: %d\n", targs[i].start);
        // printf("end: %d\n", targs[i].end);
        targs[i].min_time = min_time;
        targs[i].count = count;
        targs[i].mutex = mutex;
        // printf("creating thread %d\n", i);
        pthread_create(&threads[i], NULL, countsort_timethread, &targs[i]);
        pthread_join(threads[i], NULL);
    }
    printf("finished\n");

    // Wait for all threads to finish
    for (int i = 0; i < t; i++) {
        pthread_join(threads[i], NULL);
    }
    //printf("threads joined\n");
    // Store the cumulative count
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }

    // Build the output array
    type* output = (type*)malloc(n * sizeof(type));
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    for (int i = n - 1; i >= 0; i--) {
        int index = array[i].timestamp - min_time;
        output[--count[index]] = array[i];
    }

    // Copy the sorted output array to the original array
    for (int i = 0; i < n; i++) {
        array[i] = output[i];
    }

    // Free allocated memory
    free(count);
    free(output);
    for (int i = 0; i < range; i++) {
        pthread_mutex_destroy(&mutex[i]);
    }
    free(mutex);
}