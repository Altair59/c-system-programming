#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 64 * 64

/* interesting test case for smalloc and sfree. */
/* case description: 
	step 1. smalloc handling case when num_bytes is larger than SIZE
	step 2. smalloc handling appends to empty (initial) allocated_list
	step 3. smalloc handling appends to non-empty allocated_list
	step 4. sfree handling appends to empty freelist
	step 5. sfree handling appends to non-empty freelist
	step 6. sfree handling case when all space are already freed

*/

int main(void) {

    mem_init(SIZE);

    char *ptrs[10];
    int i;

    /* Call smalloc 5 times */

    /* STEP1: assign over-valued space to ptrs[0] */
    ptrs[0] = smalloc(SIZE * 4);

    /* STEP 2,3: assign each block with SIZE/4 space */
    for(i = 0; i <4; i++) {
        int num_bytes = SIZE / 4;

        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    /* STEP 4,5: free all blocks */
    printf("freeing %p result = %d\n", ptrs[0], sfree(ptrs[0]));
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));
    printf("freeing %p result = %d\n", ptrs[2], sfree(ptrs[2]));
    printf("freeing %p result = %d\n", ptrs[3], sfree(ptrs[3]));
 
    /* STEP 6: free ptrs[1] after it's already freed */
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));    
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

    mem_clean();
    return 0;
}