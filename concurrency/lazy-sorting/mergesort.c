#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "main.h"

int cmp(type* a, type* b, enum comparison_mode mode) {
    // Return negative if a comes before b.
    switch(mode) {
        case name:
            return strcmp(a->name, b->name);
        case id:
            return a->id - b->id;
        case timestamp:
            return difftime(a->timestamp, b->timestamp);
    }
	return 0;
}

void merge(type* a, type* b, int n1, int n2, enum comparison_mode mode) {
	type c[n1 + n2];
	int ap = 0, bp = 0;
	for(int i = 0; i < n1 + n2; i++) {
		if(cmp(a + ap, b + bp, mode) < 0) {
			if(ap < n1) c[i] = a[ap++];
			else c[i] = b[bp++];
		}
		else {
			if(bp < n2) c[i] = b[bp++];
			else c[i] = a[ap++];
		}
	}
	for(int i = 0; i < n1 + n2; i++) a[i] = c[i];
}

void* mergesort(void* args) {
    type* a = ((SortArgs*)args)->array;
    int n = ((SortArgs*)args)->size;
    enum comparison_mode mode = ((SortArgs*)args)->mode;
	if(n == 1) return NULL;
	long long int k = (long long int)(n/2);
    SortArgs args1 = {a, k, mode};
    SortArgs args2 = {a + k, n - k, mode};
	mergesort(&args1);
	mergesort(&args2);
	merge(a, a + k, k, n - k, mode);
	return NULL;
}