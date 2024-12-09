#define _GNU_SOURCE 1
#define _XOPEN_SOURCE 1
#define nthreads 16
#define chunk_size 3

#include <time.h>

// Structure to represent a file.
struct file {
    char name[8];
    int id;
    time_t timestamp;
};

typedef struct file type;

// Comparison modes to sort by.
enum comparison_mode {
    name, id, timestamp
};

// Arguments to pass to the sorting functions.
typedef struct SortArgs {
    type* array;
    int size;
    enum comparison_mode mode;
} SortArgs;

void *mergesort(void* args), *countsort(void* args);