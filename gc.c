#include <stdlib.h>
#include "gc.h"

typedef struct blokk {
    void *pointer;
    int refCount;
    struct blokk *next; // Added a pointer to the next element
} blokk_t;

struct gc {
    blokk_t *head;
    int numitems;
};

// Initializes the garbage collector
gc_t *gc_init(void) {
    gc_t *gc = (gc_t *)malloc(sizeof(gc_t));
    if (gc != NULL) {
        gc->head = NULL;
        gc->numitems = 0;
    }
    return gc;
}

// Allocates a contiguous piece of garbage-collected memory of a certain size
void *gc_malloc(gc_t *gc, size_t size) {
    void *mem = malloc(size);
    if (mem != NULL) {
        blokk_t *block = (blokk_t *)malloc(sizeof(blokk_t));
        if (block != NULL) {
            block->pointer = mem;
            block->refCount = 1;
            block->next = gc->head;
            gc->head = block;
            gc->numitems++;
        }
    }
    return mem;
}

// Registers use of a certain piece of memory by incrementing the reference counter
void gc_register(gc_t *gc, void *ptr) {
    blokk_t *block = gc->head;
    while (block != NULL) {
        if (block->pointer == ptr) {
            block->refCount++;
            break;
        }
        block = block->next;
    }
}

// Unregisters use of the memory pointed to by ptr. If the reference counter is zero,
// the memory can be safely freed; otherwise, decrement the reference count.
void gc_free(gc_t *gc, void *ptr) {
    blokk_t *prev = NULL;
    blokk_t *current = gc->head;
    
    while (current != NULL) {
        if (current->pointer == ptr) {
            if (current->refCount > 0) {
                current->refCount--;
                if (current->refCount == 0) {
                    // If the reference count is zero, free the memory
                    free(current->pointer);
                    if (prev != NULL) {
                        prev->next = current->next;
                    } else {
                        gc->head = current->next;
                    }
                    free(current);
                    gc->numitems--;
                }
            }
            break;
        }
        prev = current;
        current = current->next;
    }
}
