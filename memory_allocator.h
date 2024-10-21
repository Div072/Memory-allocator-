#include <locale.h>
#include <stddef.h>
#include <stdio.h>
#include <windows.h>  // For VirtualAlloc and VirtualFree

#define INITIAL_POOL_SIZE 1024 * 1024  // Initial memory pool size (1MB)
#define ALIGNMENT 8

typedef struct block_header {
    size_t size;
    int free;
    struct block_header *next;
    struct block_header *prev; //previous node
} block_header_t;

block_header_t *free_list = NULL;  // Start with no free list


void traverse_memory_list() {
    block_header_t *curr = free_list;
    while (curr != NULL) {
        printf("memory: %p, %p, size: %zu, free: %d\n", curr, curr->prev,curr->size, curr->free);
        curr = curr->next;
    }
}
void defragmentation(){
    printf("Defragmenting memory...\n");
    printf("Before..\n");
    traverse_memory_list();
    block_header_t *current = free_list;
    while (current != NULL) {
        if(current->prev !=NULL && current->prev->free == 1 && current->free == 1) {

            int size_z = current->prev->size + current->size;
            if (size_z+sizeof(block_header_t)<INITIAL_POOL_SIZE) {
                current->prev->size = size_z;
                current->prev->next = current->next;
            }
        }
        current = current->next;
    }
    printf("After..\n");
    traverse_memory_list();
}
// Initialize the memory pool using VirtualAlloc()
void *initialize_memory_pool(size_t size) {
    void *memory_pool = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (memory_pool == NULL) {
        printf("VirtualAlloc failed: %ld\n", GetLastError());
        return NULL;
    }

    // Set the free list to the start of the allocated memory pool
    block_header_t *initial_block = (block_header_t *)memory_pool;
    initial_block->size = size - sizeof(block_header_t);
    initial_block->free = 1;
    initial_block->next = NULL;
    initial_block->prev = NULL;

    return initial_block;
}

// Function to expand the memory pool when it's full
void expand_memory_pool(size_t size) {
    // Allocate a new memory block using VirtualAlloc()

    block_header_t *new_block = initialize_memory_pool(size);
    if (new_block == NULL) {
        printf("Failed to expand memory pool.\n");
        return;
    }

    // Find the last block in the free list and link the new block to it
    block_header_t *current = free_list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_block;// Link the new block to the free list
    new_block->prev = current; // link prev of new to current.
}

void *my_malloc(size_t size) {
   // size = align(size);  // Align the requested size
    if(size+sizeof(block_header_t)>INITIAL_POOL_SIZE) {
        printf("tried to allocate more memory then memory pool size\n");
        return NULL;
    }
    block_header_t *current = free_list;

    // Search for a suitable free block
    while (current != NULL) {
        if (current->free && current->size >= size) {
            // If the block is larger than required, split it
            if (current->size >= size + sizeof(block_header_t)) {
                block_header_t *new_block = (block_header_t *)((char *)current + sizeof(block_header_t) + size);
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->free = 1;
                new_block->next = current->next;
                new_block->prev = current;

                // Update the current block
                current->size = size;
                current->next = new_block;
                current->free = 0;
            }

            current->free = 0;
            return (void *)(current + 1);  // Return the memory after the block header
        }
        current = current->next;
    }

    // If no suitable block was found, expand the memory pool
    printf("Memory pool exhausted, expanding...\n");
    expand_memory_pool(INITIAL_POOL_SIZE);
    defragmentation();
    return my_malloc(size);  // Try allocating again after expanding
}

void *my_free(void *ptr) {
    if (!ptr) return;

    // Correct pointer arithmetic to access block header
    block_header_t *header = (block_header_t *)&ptr - 1;
    header->free = 1;
    defragmentation();
    return NULL;
    // Add logic to coalesce adjacent free blocks here if needed
}


// Clean up and release the memory pool using VirtualFree()
void free_memory_pool() {
    printf("Freeing memory pool...\n");
    block_header_t *current = free_list;
    // Optional: Check if free_list is valid
    if (!current) {
        return;  // Nothing to free
    }
    while(current!=NULL) {
        current->free= 1;
        block_header_t* next = current->next;
        VirtualFree(current,current->size+sizeof(block_header_t), MEM_RELEASE);
        current = next;
    }



    // Optional: Reset free_list after freeing
    
}

void init_malloc() {
    // Initialize the first memory pool
    free_list = initialize_memory_pool(INITIAL_POOL_SIZE);
}

