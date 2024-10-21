#include <stdio.h>
#include "./memory_allocator.h"
int main() {

    init_malloc();
    // Example allocations
    int *p1 = (int*)my_malloc(sizeof(int)*10);

    if(p1) {
        printf("Allocated 100 bytes at %p\n", p1);
        for(int i=0;i<10;i++) {
            p1[i] = i;
            printf("Element at p[%d]: %d\n",p1[i],i);
        }
    }

    int *p2 = my_malloc(1024*1024);
    if(p2) {
        printf("Allocated 1024*1024 bytes at %p\n", p2);
    }
    // Free memory and try re-allocating
    p1 = my_free(p1);

    // Clean up and release the memory pool
    free_memory_pool();
    return 0;
}