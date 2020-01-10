#include <stdio.h>
 
void hunt(int num_rows,int num_cols, int arr[num_rows][num_cols], int visited[num_rows][num_cols], int row, int col);

int isValid(int num_rows,int num_cols, int arr[num_rows][num_cols], int visited[num_rows][num_cols], int row, int col);

/* Reads the image from the open file fp into the two-dimensional array arr 
 * num_rows and num_cols specify the dimensions of arr
 */
void read_image(int num_rows, int num_cols, 
                int arr[num_rows][num_cols], FILE *fp) {
	for(int row=0; row<num_rows; row++)
        for(int col=0; col<num_cols; col++)
            fscanf(fp, "%d", &arr[row][col]);
}

/* Print to standard output the contents of the array arr */
void print_image(int num_rows, int num_cols, int arr[num_rows][num_cols]) {
	int row,col = 0;
	for(row=0; row<num_rows; row++){
        for(col=0; col<num_cols; col++){
           	if (col != num_cols-1){
           		printf ("%d ", arr[row][col]);
           	} else {
           		printf ("%d", arr[row][col]);
           	}
        }
        printf("\n");
    }
}

/* TODO: Write the count_cells function */
int count_cells(int num_rows,int num_cols, int arr[num_rows][num_cols]){
	int count = 0;

	int visited[num_rows][num_cols];
	for (int i=0;i<num_rows;i++)
		for (int j=0;j<num_cols;j++)
			visited[i][j] = 0;

	for (int row=0;row<num_rows;row++){
		for (int col=0;col<num_cols;col++){
			if (arr[row][col] == 255 && visited[row][col] == 0){
				hunt(num_rows,num_cols, arr, visited, row, col);
				count++;
			}
		}
	}
	return count;
}


void hunt(int num_rows, int num_cols, int arr[num_rows][num_cols], int visited[num_rows][num_cols], int row, int col){
	int x_cor[4] = {1,-1,0,0};
	int y_cor[4] = {0,0,-1,+1};
	visited[row][col] = 1;
	for (int k=0;k<4;k++){
		if (isValid(num_rows,num_cols,arr,visited,row+y_cor[k],col+x_cor[k])){
			hunt(num_rows, num_cols, arr,visited,row+y_cor[k],col+x_cor[k]);
		}
	}
	
}

int isValid(int num_rows, int num_cols, int arr[num_rows][num_cols], int visited[num_rows][num_cols], int row, int col){
	return arr[row][col] == 255 && visited[row][col] == 0 && 
		col >= 0 && col < num_cols && row >=0 && row < num_rows;
}


