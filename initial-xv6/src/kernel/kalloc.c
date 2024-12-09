// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run
{
  struct run *next;
};

struct
{
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct{
  struct spinlock lock;
  int refcount[PHYSTOP/PGSIZE];
}ref;

// increases the reference count of a page
void incref(void *pa)
{
  int idx = PA2IDX(pa);  
  acquire(&ref.lock);
  ref.refcount[idx]++;
  release(&ref.lock);
}


// decreases the reference count of a page
void decref(void *pa)
{
  int idx = PA2IDX(pa);
  acquire(&ref.lock);
  ref.refcount[idx]--;
  release(&ref.lock);
}


// get the reference count of a page
int getref(void* pa)
{
  int count;
  acquire(&ref.lock);
  count = ref.refcount[PA2IDX(pa)];
  release(&ref.lock);
  return count;
}



void kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&ref.lock, "ref");
  // initialise refcount array
  for(int i=0;i<(PHYSTOP/PGSIZE); i++)
    ref.refcount[i] = 0;
  freerange(end, (void *)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
  {
    // kfree decreases count , so incref before kfree to ensure non negative count
    incref(p);
    kfree(p);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");
 

  if(getref(pa) <=0)
    panic("attempting to decrement freed page");

  decref(pa);

  // if ref >0, the page is being used by another process, do not free it
  if(getref(pa)>0)
    return;

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);
  acquire(&kmem.lock);
  r = (struct run *)pa;
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r)
  {
    kmem.freelist = r->next;
  }
  release(&kmem.lock);

  // incref since a new page is being allocated
  if (r){
    incref(r);
    memset((char *)r, 5, PGSIZE); // fill with junk
  }
  return (void *)r;
}
