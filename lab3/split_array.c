#include <stdio.h>
#include <stdlib.h>


int **split_array(const int *s, int length);

int *build_array(char **strs, int size);

/* Return a pointer to an array of two dynamically allocated arrays of ints.
   The first array contains the elements of the input array s that are
   at even indices.  The second array contains the elements of the input
   array s that are at odd indices.

   Do not allocate any more memory than necessary.
*/
int **split_array(const int *s, int length) {
      int first_size,second_size;

      if (length % 2 == 0){
        first_size = length / 2;
        second_size = length / 2;
      } else {
        first_size = (length+1) / 2;
        second_size = (length-1) / 2;
      }

      int *first = malloc(sizeof(int)*first_size);
      int *second = malloc(sizeof(int)*second_size);
      int first_index = 0;
      int second_index = 0;

      for (int i=0;i<length;i++){
        if (i%2 == 1){
          second[second_index] = s[i];
          second_index++;
        } else{
          first[first_index] = s[i];
          first_index++;
        }
      }

      int** array = malloc(sizeof(int*)*2);
      array[0] = first;
      array[1] = second;
      return array;
}

/* Return a pointer to an array of ints with size elements.
   - strs is an array of strings where each element is the string
     representation of an integer.
   - size is the size of the array
 */

int *build_array(char **strs, int size) {
      int *s = malloc(sizeof(int)*(size-1));
      for (int i=1;i<size;i++){
        s[i-1] = strtol(strs[i],NULL,10);
      }
      return s;

}


int main(int argc, char **argv) {
    /* Replace the comments in the next two lines with the appropriate
       arguments.  Do not add any additional lines of code to the main
       function or make other changes.
     */
    int *full_array = build_array(argv,argc);
    int **result = split_array(full_array, argc-1);

    printf("Original array:\n");
    for (int i = 0; i < argc - 1; i++) {
        printf("%d ", full_array[i]);
    }
    printf("\n");

    printf("result[0]:\n");
    for (int i = 0; i < argc/2; i++) {
        printf("%d ", result[0][i]);
    }
    printf("\n");

    printf("result[1]:\n");
    for (int i = 0; i < (argc - 1)/2; i++) {
        printf("%d ", result[1][i]);
    }
    printf("\n");
    free(full_array);
    free(result[0]);
    free(result[1]);
    free(result);
    return 0;
}
