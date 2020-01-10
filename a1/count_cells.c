#include <stdio.h>
#include <stdlib.h>
// function prototype
void read_image(int num_rows, int num_cols, int arr[num_rows][num_cols], FILE *fp);
                
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]);

int count_cells(int num_rows,int num_cols, int arr[num_rows][num_cols]);


// Remember to include the function prototype for count_cells

int main(int argc, char **argv) {
    // Print a message to stderr and exit with an argument of 1 if there are
    // not the right number of parameters, or the second argument is not -p
	
    int illegal = 0; //command line input illegal flag
	
    int print = 0; // print image flag
	
    if (argc != 2 && argc != 3){ // check number of command line argument
		illegal = 1;
		fprintf(stderr, "%s\n", "Usage: count_cells <imagefile.txt> [-p]");
	} 
	
	const char *filename = argv[1];
	FILE* fp;

	if (argc == 3){ // check format
		if (argv[2][0] != '-' || argv[2][1] != 'p' || argv[2][2] != '\0'){
			illegal = 1;
			fprintf(stderr, "%s\n", "Usage: count_cells <imagefile.txt> [-p]");
		}  else {
			print = 1;
		}
	}

    if (illegal == 1)
    	exit(1);
    
    int num_rows = 0;
    int num_cols = 0;
    
    if((fp = fopen(filename, "r")) == NULL)
        exit(1);

    if(fscanf(fp, "%d%d\n", &num_rows, &num_cols) != 2) // read num_rows and num_vols
        exit(1);
    
    if (num_rows < 1 || num_cols < 1)
        exit(1);

    int arr[num_rows][num_cols];

    read_image(num_rows, num_cols, arr, fp);

    if (print == 1)
    	print_image(num_rows,num_cols,arr);

    int count = count_cells(num_rows,num_cols,arr);

    printf("Number of Cells is %d\n", count);

	fclose(fp);

	




    return 0;
}
