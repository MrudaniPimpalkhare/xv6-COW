#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int n; scanf("%d", &n);
    // struct file files[n];
    struct file* files = (struct file*) malloc(n * sizeof(struct file));
    for(int i = 0; i < n; i++) {
        char id[10], timestamp[40];
        scanf("%s%s%s", files[i].name, id, timestamp);
        files[i].id = atoi(id);
        // parse the timestamp
        struct tm tm;
        strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm);
        if(strptime(timestamp, "%Y-%m-%dT%H:%M:%S", &tm) == NULL) {
            fprintf(stderr, "Could not parse the timestamp: %s\n", timestamp);
            exit(1);
        }
        tm.tm_isdst = -1;
        files[i].timestamp = mktime(&tm);
    }

    char buf[100];
    enum comparison_mode mode;
    scanf("%s", buf);
    if(strcmp(buf, "Name") == 0) mode = name;
    else if(strcmp(buf, "ID") == 0) mode = id;
    else if(strcmp(buf, "Timestamp") == 0) mode = timestamp;
    else {
        fprintf(stderr, "Unknown mode: %s\n", buf);
        exit(1);
    }

    SortArgs args = {files, n, mode};
    if(n <= 42) countsort(&args);
    else mergesort(&args);
    // mergesort(&args);
    printf("%s\n", buf);
    for(int i = 0; i < n; i++) {
        char tstring[40];
        // format the timestamp
        strftime(tstring, 40, "%Y-%m-%dT%H:%M:%S", localtime(&files[i].timestamp));
        printf("%s %d %s\n", files[i].name, files[i].id, tstring);
    }
}