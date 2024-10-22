#include <stdio.h>
#include "./memory_allocator.h"
int main() {

    init_malloc();
    // Example allocations
    int *p1 = (int*)my_malloc(sizeof(int)*10);

    if(p1) {
        for(int i=0;i<10;i++) {
            p1[i] = i;
            printf("Element at p[%d]: %d\n",p1[i],i);
        }
    }

    int *p2 = (int*)my_malloc(sizeof(int)*20);


    // Free memory and try re-allocating
    p1 = my_free(p1);
    my_realloac(0,p2);

    // Clean up and release the memory pool
    free_memory_pool();
    return 0;
}