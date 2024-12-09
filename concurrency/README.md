# LAZY Sorting

## Assumptions
- File IDs can range from 0 to 100000.
- File IDs are unique.
- File names are unique.
- File names can be at most 7 characters long.

## Distributed merge sort
The idea is that since for an array, we are sorting its left and right subarrays, why not have those 2 sorts on separate threads? There is no shared data between those arrays so locks aren't required either.

Approach 1: Naive
```
mergesort:
    mergesort left subarray on a new thread;
    mergesort right subarray on a new thread;
    join threads;
    merge;
```
However, the main thread is doing nothing until the new threads are joined back. Also, this creates a large number of threads, which is linear with respect to input size.

Approach 2:
```
mergesort:
    mergesort left subarray on a new thread;
    mergesort right subarray;
    join thread;
    merge;
```
This **halves** the number of threads being created and makes sure no thread remains idle. However, the number of threads being created is still large.

Approach 3: Limiting the number of threads
```
num_threads = 0;
max_threads = 64;

mergesort:
    if num_threads <= max_threads:
        num_threads++;
        mergesort left subarray on a new thread;
        mergesort right subarray;
        join thread;
        num_threads--;
    else:
        mergesort left subarray;
        mergesort right subarray;
    merge;
```
Of course, a lock must be used to make sure there are no race conditions when it comes to modifying the `num_threads` variable.

There is no point of making multithreaded programs if all the threads run on the same proessor. It will not be faster than a single-threaded sequential program.

Turns out that this is exactly the method specified in https://rachitvasudeva.medium.com/parallel-merge-sort-algorithm-e8175ab60e7#da33. 
Since the recursion tree descends at the same rate at all nodes (because of the concurrency), the threading gets limited to a particular level in the tree.

**How many threads to limit to?**
On first thought, we could limit it to 32 threads (maximum number of threads supported by hardware for parallel execution) 
for a 16-core processor but it is not necessary that the first 32 spawned threads are evenly distributed across all cores. 
Therefore, it might be beneficial to increase the threshold so that on spawning a new thread, 
it gets scheduled on a completely new processor, allowing for better parallelism. 
In our approach, we have decided to go with a maximum of 128 threads.

## Distributed count sort
### By name
Since there are 128^128 possible ASCII strings of length 128, there are way too many strings to do a count sort, as it will involve iterating through the entire search space.
We will limit the string length to 7 (a string of length 8 can also be hashed into an `unsigned long long int`, yes, but memory allocation for that huge of an array fails), and only allow lowercase English alphabets. This restricts the search space to 26^7 strings.

Also, directly allocating the count array of size 26^7 will prove inefficient at times. Let's say all strings start with an 'a', then all the non-zero elements in the count array are restricted to a small part of it. This problem can be somewhat fixed by dividing the count array into chunks and only allocating memory for a chunk when we find an element that belongs to that chunk. Implementation wise, this would mean creating an array of integer pointers. Allocate memory to each pointer if you find an element belonging to that chunk.

In our implementation, we allocate a 2D array of size 26 by 26^6. Use the first character to decide which of the 26 it goes to, and hash the rest to decide its position in the array. Therefore, this approach consumes an order of 26^6 bytes of memory at best and 26^7 at worst as compared to the naive approach which always takes up 26^7 bytes. We could go deeper by doing a 3D array but after a while it gets tedious to deal with so many levels of pointers as there is no convenient way to do it in C.

**Parallelising:**
Have each thread look at a different part of the array. For characteristics that are unique (like ID and name), no two threads will access the same element in the count array, so we need not use any locks.

# LAZY read-write
## Assumptions
- All integer values (User and file IDs and time) fit in the 32-bit range.
- Requests to files whose file ID is more than `n` will be rejected initially as an erroneous request, just like requests which do not have a valid mode like `3 4 RSEF 4`.

## Implementation
Each request and file is modelled as a `struct`. Time is modelled by a global variable `m_time`. Creating threads for handling requests and maintaining the timer is done by the main thread.
At each timer update, the main thread spawns threads for handling requests that come in at that time.

A worker thread, in turn, at every timer update, checks the state of the request. If it's able to take up the request, it sets its state to "taken up".
Otherwise, it checks at the next timer update. It sleeps between these timer updates. The main thread ensures all worker threads are sleeping before a timer update.
If the request is done processing, the thread exits. To maintain synchronization between the threads, locks and condition variables have been used.
Since each `struct request` is only being handled by one thread, we need not lock its properties.
But since a `struct file` is being handled by multiple requests (hence multiple threads), we need to lock its properties.