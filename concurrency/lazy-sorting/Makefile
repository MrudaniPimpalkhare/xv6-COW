TARGETS := main.c

ifdef mode
	ifeq ($(mode), distributed)
		TARGETS += distributed_mergesort.c
		TARGETS += countsort.c
		TARGETS += dist_countsort_id.c
		TARGETS += dist_countsort_time.c 
		TARGETS += dist_countsort_name.c
	else
		TARGETS += mergesort.c
		TARGETS += countsort.c
		TARGETS += countsort_name.c
		TARGETS += countsort_time.c
		TARGETS += countsort_id.c
	endif
else
	TARGETS += mergesort.c
	TARGETS += countsort.c
	TARGETS += countsort_name.c
	TARGETS += countsort_id.c
	TARGETS += countsort_time.c
endif

default: $(TARGETS)
	gcc -g $(TARGETS) -o lazy-sort -lpthread -Wall -pedantic

.PHONY: clean
clean:
	rm lazy-sort