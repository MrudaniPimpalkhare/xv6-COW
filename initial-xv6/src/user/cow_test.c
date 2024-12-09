#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_PAGES 10
#define PAGE_SIZE 4096

void read_only_test(char *addr) {
  int pid = fork();
  if (pid == 0) {
    // Child process
    for (int i = 0; i < NUM_PAGES * PAGE_SIZE; i += PAGE_SIZE) {
      (void)addr[i]; // Access each page to ensure they are mapped
    }
    exit(0);
  } else {
    // Parent process
    wait(0);
    int page_faults = get_page_fault_count();
    printf("Read-Only Test: Page Faults = %d\n", page_faults);
  }
}

void modify_test(char *addr) {
  int pid = fork();
  if (pid == 0) {
    // Child process
    for (int i = 0; i < NUM_PAGES * PAGE_SIZE; i += PAGE_SIZE) {
      addr[i] = 'A'; // Write to each page to trigger COW
    }
    exit(0);
  } else {
    // Parent process
    wait(0);
    printf("Modify Test: Page Faults = %d\n", get_page_fault_count());
  }
}

int main() {
  printf("Initial Page Faults: %d\n", get_page_fault_count());

  // Allocate memory in the parent process, which will be shared with the child processes
  char *addr = sbrk(NUM_PAGES * PAGE_SIZE);

  read_only_test(addr);
  printf("Page Faults after Read-Only Test: %d\n", get_page_fault_count());

  modify_test(addr);
  exit(0);
}