#include "main.h"
#include "countsort.h"
#include <stdio.h>
#include <limits.h>

void* countsort(void* args) {
    enum comparison_mode mode = ((SortArgs*)args)->mode;
    if(mode == name) {
        countsort_name(args);
    }
    else if(mode == id) {
        printf("countsort_id\n");
        countsort_id(args);
    }
    else if(mode == timestamp) {
        countsort_timestamp(args);
    }
    return NULL;
}