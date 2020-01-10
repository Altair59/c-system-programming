#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;


void *smalloc(unsigned int nbytes) {
	// the starting address of the memory to be allocated
    void *start = NULL;
    // initialize node pointers to traverse freelist
    struct block *curr_free = freelist, *prev_free = NULL;
    // round nbytes to nearest greater 8 multiples for "aligned on an 8-byte boundary"
    nbytes = roundto8(nbytes);

    // traverse freelist to look for a block that is of at least nbytes size
    while (curr_free != NULL && curr_free->size < nbytes){
        // record prev_free and go to next node
        prev_free = curr_free;
        curr_free = curr_free->next;
    }
    
    // if curr_free is NULL
    if (curr_free == NULL){
        return start;
    }
    // if find a block of exactly nbytes size
    else if (curr_free->size == nbytes){
        // assign start
        start = (void *)curr_free->addr;
            
        // updating freelist and allocated_list
        // delete node from freelist
        if (prev_free == NULL){
            freelist = curr_free->next;
        } else {
            prev_free->next = curr_free->next;
        }
        // insert the node deleted to the front of allocated_list
        curr_free->next = allocated_list;
        allocated_list = curr_free;
    } 
    // if find a block of larger than nbytes size
    else if (curr_free->size > nbytes){
        // assign start
        start = (void *)curr_free->addr;

        // updating freelist and allocated_list
        //insert nbytes space from splitting curr_free to the front of allocated_list
        struct block *temp = (struct block *)malloc(sizeof(struct block));
        temp->addr = curr_free->addr;
        temp->size = nbytes;
        temp->next = allocated_list;
        allocated_list = temp;
        // modify curr_free in freelist due to split
        curr_free->addr = curr_free->addr + nbytes;
        curr_free->size = curr_free->size - nbytes;
    }
    
    // return start of allocated space
    return start;
}


int sfree(void *addr) {
	// initialize node pointers to traverse freelist
    struct block *curr_alloc = allocated_list, *prev_alloc = NULL;

    // traverse allocated_list to look for the block starts with addr
    while (curr_alloc != NULL && curr_alloc->addr != addr){
        // record prev_alloc and go to next node
        prev_alloc = curr_alloc;
        curr_alloc = curr_alloc->next;
    }

    // if curr_alloc is NULL
    if (curr_alloc == NULL){
        return -1;
    }
    //else
    else {
        // delete the block from allocated_list
        if (prev_alloc == NULL){
            allocated_list = curr_alloc->next;
        } else {
            prev_alloc->next = curr_alloc->next;
        }

        // attach the block to freelist
        // traverse the freelist to find the block with largest address smaller than addr
        struct block * prev_temp = NULL;
        struct block * curr_temp= freelist;
        while (curr_temp != NULL && curr_temp->addr < addr) {
            prev_temp = curr_temp;
            curr_temp = curr_temp->next;
        }
        // if curr_temp is empty
        if (curr_temp == NULL){
            // if freelist is empty
            if (prev_temp == NULL){
                freelist = curr_alloc;
            }
            // if reached end of freelist
            else {
                prev_temp->next = curr_alloc;
            }
        }
        // if the target block is found
        else {
            // if at head
            if (prev_temp == NULL){
                curr_alloc->next = curr_temp;
                freelist = curr_alloc;
            }
            // if not at head
            else {
                prev_temp->next = curr_alloc;
                curr_alloc->next = curr_temp;
            }
        }
        return 0;
    }
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

	// Initialize freelist by mem
    freelist = (struct block*)malloc(sizeof(struct block));
    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;

    // Initialize empty allocated_list
    allocated_list = NULL;
        
}

void mem_clean(){
    // traverse each used list to free all blocks
    struct block * current = allocated_list;
    while (current != NULL) {
        struct block * dummy = current->next;    
        free(current);
        current = dummy;
    } 

    current = freelist;
    while (current != NULL) {
        struct block * dummy = current->next;    
        free(current);
        current = dummy;
    }
}

unsigned int roundto8(unsigned int num){
    int remainder = num % 8;
    if (remainder == 0){
        return num;
    } else {
        return (num + 8 - remainder);
    }
}

