#include "main.h"
#include "countsort.h"

#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int NTHREADS = 16;
// unsigned long long int countsize = 26LL * 26LL * 26LL * 26LL * 26LL * 26LL * 26LL; // 26^7, roughly 8 GB for the array
unsigned long long int countsize = 26LL * 26LL * 26LL * 26LL * 26LL * 26LL;

type*** count;

pthread_mutex_t countlocks[26] = {PTHREAD_MUTEX_INITIALIZER};

struct sortArgs2 {
    int lower, upper;
    type* array;
};

void set(char* str, type* element) {
    if(str[0] < 'a' || str[0] > 'z') return;
    pthread_mutex_lock(&countlocks[str[0] - 'a']);
    if(count[str[0] - 'a'] == NULL) {
        count[str[0] - 'a'] = calloc(countsize, sizeof(type*));
    }
    pthread_mutex_unlock(&countlocks[str[0] - 'a']);
    unsigned long long int hash = 0;
    for(int i = 1; i < strlen(str); i++) {
        hash += (25 * hash) + str[i] - 'a';
    }
    assert(count[str[0] - 'a'][hash] == NULL);
    count[str[0] - 'a'][hash] = element;
}

void* sorterworker(void* args) {
    int lower = ((struct sortArgs2*)args)->lower;
    int upper = ((struct sortArgs2*)args)->upper;
    type* array = ((struct sortArgs2*)args)->array;
    for(int i = lower; i < upper; i++) {
        // set(array[i].name, array + i);
        if(array[i].name[0] < 'a' || array[i].name[0] > 'z') return NULL;
        pthread_mutex_lock(&countlocks[array[i].name[0] - 'a']);
        if(count[array[i].name[0] - 'a'] == NULL) {
            count[array[i].name[0] - 'a'] = calloc(countsize, sizeof(type*));
        }
        pthread_mutex_unlock(&countlocks[array[i].name[0] - 'a']);
        unsigned long long int hash = 0;
        for(int j = 1; j < strlen(array[i].name); j++) {
            hash += (25 * hash) + array[i].name[j] - 'a';
        }
        // pthread_mutex_lock(&countlocks[array[i].name[0] - 'a']);
        assert(count[array[i].name[0] - 'a'][hash] == NULL);
        // if(count[array[i].name[0] - 'a'][hash] != NULL) return;
        // fprintf(stderr, "setting element %llu to %s\n", hash, array[i].name);
        count[array[i].name[0] - 'a'][hash] = array + i;
        // pthread_mutex_unlock(&countlocks[array[i].name[0] - 'a']);
    }
    if(lower == upper) {
        // set(array[lower].name, array + lower);
        int i = lower;
        if(array[i].name[0] < 'a' || array[i].name[0] > 'z') return NULL;
        pthread_mutex_lock(&countlocks[array[i].name[0] - 'a']);
        if(count[array[i].name[0] - 'a'] == NULL) {
            count[array[i].name[0] - 'a'] = calloc(countsize, sizeof(type*));
        }
        pthread_mutex_unlock(&countlocks[array[i].name[0] - 'a']);
        unsigned long long int hash = 0;
        for(int j = 1; j < strlen(array[i].name); j++) {
            hash += (25 * hash) + array[i].name[j] - 'a';
        }
        // pthread_mutex_lock(&countlocks[array[i].name[0] - 'a']);
        assert(count[array[i].name[0] - 'a'][hash] == NULL);
        // fprintf(stderr, "setting element %llu to %s\n", hash, array[i].name);
        count[array[i].name[0] - 'a'][hash] = array + i;
        // pthread_mutex_unlock(&countlocks[array[i].name[0] - 'a']);
    }
    return NULL;
}

void countsort_name(void* args) {
    int n = ((SortArgs*)args)->size;
    type* a = malloc(n * sizeof(type));
    for(int i = 0; i < n; i++) {
        a[i] = (((SortArgs*)args)->array)[i];
    }
    enum comparison_mode mode = ((SortArgs*)args)->mode;
    count = (type***) calloc(26, sizeof(type**));
    assert(mode == name);
    if(n < NTHREADS) NTHREADS = n;
    pthread_t threads[NTHREADS + 1];
    struct sortArgs2 argslist[NTHREADS + 1];
    for(int i = 0; i < NTHREADS + 1; i++) {
        int lower = i * n / NTHREADS;
        int upper = (i + 1) * n / NTHREADS;
        if(upper > n) upper = n;
        // struct sortArgs2 args = {lower, upper, a}; // EXCLUDE UPPER
        argslist[i] = (struct sortArgs2) {lower, upper, a};
        // fprintf(stderr, "creating thread %d sorting elements from %d inclusive to %d \n", i, lower, upper);
        pthread_create(&threads[i], NULL, sorterworker, argslist + i);
        pthread_join(threads[i], NULL);
    }
    for(int i = 0; i < NTHREADS + 1; i++) pthread_join(threads[i], NULL);
    int index = 0;
    for(int i = 0; i < 26; i++) {
        if(count[i] == NULL) continue;
        for(unsigned long long int j = 0; j < countsize; j++) {
            if(count[i][j]) {
                (((SortArgs*)args)->array)[index++] = *(count[i][j]);
            }
        }
    }
    // for(int i = 0; i < n; i++) {
    //     (((SortArgs*)args)->array)[i] = a[i];
    // }
}

// void countsort_id(void* args) {}
// void countsort_timestamp(void* args) {}

// abcdefg