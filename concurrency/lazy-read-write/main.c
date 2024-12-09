#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

#define AC_BLACK "\x1b[30m"
#define AC_RED "\x1b[31m"
#define AC_GREEN "\x1b[32m"
#define AC_YELLOW "\x1b[33m"
#define AC_BLUE "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN "\x1b[36m"
#define AC_WHITE "\x1b[37m"
#define AC_NORMAL "\x1b[m"

int m_time = 0;
pthread_cond_t timer_update = PTHREAD_COND_INITIALIZER; // To send a signal to worker threads that the timer has incremented.

volatile int threads_sleeping = 0, threads_spawned = 0; // threads_mutex must be held to modify these.
pthread_mutex_t threads_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t thread_slept = PTHREAD_COND_INITIALIZER; // To notify the main thread that a worker thread has slept. This is so that it can sleep waiting for all worker threads to sleep.

typedef enum fileAccessMode {
    read, write, delete, none
} mode;

struct request {
    int timeReceived, fileID, userID;
    mode type;
    bool takenUp;
    int timeTakenUp;
    // Only one thread handles a request, so there's no need to lock these properties.
};

struct file {
    int id;
    int numAccesses; // The number of users currently accessing the file.
    mode state;
    pthread_mutex_t lock; // This lock must be held to change numAccesses or state.
};

struct reqNode {
    struct request request;
    struct reqNode* next;
};

struct reqNode** requests = NULL; // Array of pointers to all requests.
struct file** files = NULL; // Array of pointers to all files.
int r, w, d, n, c, T;

struct file* makeFile(int id, int numac, mode state) {
    struct file* f = malloc(sizeof(struct file));
    f->id = id;
    f->numAccesses = numac;
    f->state = state;
    f->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    return f;
}

struct reqNode* append(struct reqNode* head, struct request req) {
    // Append a request to the end of the linked list, and return its head.
    // Return value must be used if the list to be appended is empty. Otherwise, it can be ignored.
    if(head == NULL) {    
        head = malloc(sizeof(struct reqNode));
        head->request = req;
        head->next = NULL;
        return head;
    }
    struct reqNode* m = head;
    while(head->next != NULL) {
        head = head->next;
    }
    struct reqNode* N = malloc(sizeof(struct reqNode));
    N->request = req;
    N->next = NULL;
    head->next = N;
    return m;
}

void* handleRequest(void* arg) {
    struct request* req = (struct request*)arg;
    int times[] = {r, w, d};
    struct file* targetfile = files[req->fileID - 1];
    pthread_mutex_lock(&targetfile->lock);
    while(m_time == req->timeReceived) {
        printf("%sUser %d has made a request for performing %s on file %d at %d seconds%s\n", AC_YELLOW, req->userID, req->type == read ? "READ" : req->type == write ? "WRITE" : req->type == delete ? "DELETE" : "???", req->fileID, m_time, AC_NORMAL);
        // Make the thread sleep until the timer increments.
        // If all worker threads have slept, the timer can be incremented.
        pthread_mutex_lock(&threads_mutex);
        threads_sleeping++;
        if(threads_sleeping >= threads_spawned) pthread_cond_signal(&thread_slept); // No need to signal (on signaling, the main thread checks if the number of asleep threads is equal to the number of spawned threads) unless the last thread has slept.
        pthread_mutex_unlock(&threads_mutex);
        pthread_cond_wait(&timer_update, &targetfile->lock);
    }
    while(m_time > req->timeReceived) {
        // Check if the request has been taken up.
        // If it's taken up, check if it has been processed.
        // If it's not taken up, first check if it's been timed out.
        // If it's not timed out, check if it voilates the concurrency limit.
        // If yes, delay it. If no, see if it can be taken up.
        if(req->takenUp) {
            if(m_time >= req->timeTakenUp + times[req->type]) {
                printf("%sThe request for User %d was completed at %d seconds%s\n", AC_GREEN, req->userID, m_time, AC_NORMAL);
                // set the target file's state back upon completion of the request. depends on the type of request.
                if(req->type == write) targetfile->state = read; // Any other access to the file must be a read. No other accesses is taken care of on line 106.
                targetfile->numAccesses--;
                if(targetfile->numAccesses == 0 && req->type != delete) targetfile->state = none;
                break;
            }
            pthread_mutex_lock(&threads_mutex);
            threads_sleeping++;
            if(threads_sleeping >= threads_spawned) pthread_cond_signal(&thread_slept);
            pthread_mutex_unlock(&threads_mutex);
            pthread_cond_wait(&timer_update, &targetfile->lock);
        }
        else {
            if(m_time >= req->timeReceived + T) {
                printf("%sUser %d canceled the request due to no response at %d seconds%s\n", AC_RED, req->userID, m_time, AC_NORMAL);
                pthread_mutex_unlock(&targetfile->lock);
                break;
            }
            else {
                if(targetfile->numAccesses < c) {
                    if(req->type == read) {
                        if(targetfile->state == delete) {
                            printf("LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.\n", req->userID, m_time);
                            break;
                        }
                        else {
                            printf("%sLAZY has taken up the request of User %d at %d seconds%s\n", AC_MAGENTA, req->userID, m_time, AC_NORMAL);
                            req->takenUp = true;
                            req->timeTakenUp = m_time;
                            targetfile->state = read;
                            targetfile->numAccesses++;
                        }
                    }
                    else if(req->type == write) {
                        if(targetfile->state == delete || targetfile->state == write) {
                            // printf("LAZY has declined the request of User %d at %d seconds because an invalid/deleted file was requested.\n", req->userID, m_time);
                        }
                        else {
                            printf("%sLAZY has taken up the request of User %d at %d seconds%s\n", AC_MAGENTA, req->userID, m_time, AC_NORMAL);
                            req->takenUp = true;
                            req->timeTakenUp = m_time;
                            targetfile->state = write;
                            targetfile->numAccesses++;
                        }
                    }
                    else {
                        if(targetfile->state == none && targetfile->numAccesses == 0) {
                            printf("%sLAZY has taken up the request of User %d at %d seconds%s\n", AC_MAGENTA, req->userID, m_time, AC_NORMAL);
                            req->takenUp = true;
                            req->timeTakenUp = m_time;
                            targetfile->state = delete;
                            targetfile->numAccesses++;
                        }
                    }
                }
                pthread_mutex_lock(&threads_mutex);
                threads_sleeping++;
                if(threads_sleeping >= threads_spawned) pthread_cond_signal(&thread_slept);
                pthread_mutex_unlock(&threads_mutex);
                pthread_cond_wait(&timer_update, &targetfile->lock);
            }
        }
    }
    pthread_mutex_unlock(&targetfile->lock);
    pthread_mutex_lock(&threads_mutex);
    threads_spawned--;
    pthread_cond_signal(&thread_slept);
    pthread_mutex_unlock(&threads_mutex);
    return NULL;
}

void handleAllRequests(int maxTime) {
    printf("LAZY has woken up!\n");
    while(m_time <= maxTime + T + r + w + d) {
        if(m_time <= maxTime) {
            struct reqNode* N = requests[m_time];
            while(N) {
                pthread_t thread;
                pthread_create(&thread, NULL, handleRequest, &N->request); // Create a thread for handling each request.
                pthread_mutex_lock(&threads_mutex);
                threads_spawned++;
                pthread_mutex_unlock(&threads_mutex);
                N = N->next;
            }
        }
        // Make sure all spawned threads have slept before incrementing the timer.
        pthread_mutex_lock(&threads_mutex);
        while(threads_sleeping < threads_spawned) pthread_cond_wait(&thread_slept, &threads_mutex);
        threads_sleeping = 0;
        m_time++;
        pthread_cond_broadcast(&timer_update);
        pthread_mutex_unlock(&threads_mutex);
    }
    printf("LAZY has no more pending requests and is going back to sleep!\n");
}

int main() {
    scanf("%d %d %d", &r, &w, &d);
    scanf("%d %d %d", &n, &c, &T);
    
    int numFiles = 0, numRequests = 0, maxTime = 0;
    files = calloc(n, sizeof(struct file*));
    
    char buf[100];
    fgets(buf, 100, stdin);
    fgets(buf, 100, stdin);
    while(strcmp(buf, "STOP\n") != 0) {
        int uid, fid, t;
        char mode[100];
        sscanf(buf, "%d %d %s %d", &uid, &fid, mode, &t);
        if((strcmp(mode, "READ") == 0 || strcmp(mode, "WRITE") == 0 || strcmp(mode, "DELETE") == 0) && fid <= n) {
            if(t >= maxTime) {
                maxTime = t;
                requests = realloc(requests, (maxTime+1) * sizeof(struct reqNode*));
            }
            files[fid-1] = makeFile(fid, 0, none);
            struct request req = {t, fid, uid, (mode[0] == 'R' ? read : mode[0] == 'W' ? write : mode[0] == 'D' ? delete : none), false, -1};
            requests[t] = append(requests[t], req);
        }
        else {
            fprintf(stderr, "Erroneous request!\n");
        }
        fgets(buf, 100, stdin);
    }
    handleAllRequests(maxTime);
    return 0;
}