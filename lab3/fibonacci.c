#include <stdlib.h>
#include <stdio.h>

void fib(int** fib_sequence_ptr,int count);
int fibonacci(int count);
/*
 * Define a function void fib(...) below. This function takes parameter n
 * and generates the first n values in the Fibonacci sequence.  Recall that this
 * sequence is defined as:
 *         0, 1, 1, 2, 3, 5, ... , fib[n] = fib[n-2] + fib[n-1], ...
 * The values should be stored in a dynamically-allocated array composed of
 * exactly the correct number of integers. The values should be returned
 * through a pointer parameter passed in as the first argument.
 *
 * See the main function for an example call to fib.
 * Pay attention to the expected type of fib's parameters.
 */

/* Write your solution here */


int main(int argc, char **argv) {
    /* do not change this main function */
    int count = strtol(argv[1], NULL, 10);
    int *fib_sequence;

    fib(&fib_sequence, count);
    for (int i = 0; i < count; i++) {
        printf("%d ", fib_sequence[i]);
    }
    free(fib_sequence);
    return 0;
}

void fib(int** fib_sequence_ptr,int count){
	*fib_sequence_ptr = (int *)malloc(count * sizeof(int));
	for (int i=0;i<count;i++){
		(*fib_sequence_ptr)[i] = fibonacci(i+1);
	}
}

int fibonacci(int count){
	if (count == 1){
		return 0;
	} else if (count == 2){
		return 1;
	} else {
		return fibonacci(count-1) + fibonacci(count-2);
	}
}
