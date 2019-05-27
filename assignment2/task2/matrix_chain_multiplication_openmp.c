#include <stdio.h>
#include <omp.h>
#include <limits.h>
#include <stdlib.h>
#define MAX_SIZE 100

struct Dimensions{
	int rows, cols;
};
typedef struct Dimensions Dimensions;
int min(int a, int b) {
	if (a<b) return a;
	else return b;
}

Dimensions* getMatrices(char* FileName, int* matSize) {
	FILE *matFile = fopen(FileName, "r");
	if(matFile == NULL) {
		return NULL;
	}
	int mat_num;
	fscanf(matFile, "%d", &mat_num);
	if(mat_num > MAX_SIZE){
		printf("Max size limit exceeded");
		exit(1);
	}
	Dimensions *arr = (Dimensions*)malloc(sizeof(Dimensions) * mat_num);
	int i=0;
	int rows, cols;
	while(fscanf(matFile, "%d %d", &rows, &cols) != EOF){
		arr[i].rows = rows;
		arr[i].cols = cols;
		i++;
	}
	*matSize = mat_num;
	fclose(matFile);
	return arr;
}

int main() {
	int mat_size;
	Dimensions* mat_dim = getMatrices("inp.in", &mat_size);
	int aux[MAX_SIZE][MAX_SIZE];
	for(int i=0; i<MAX_SIZE; ++i){
		for(int j=0; j<MAX_SIZE; ++j){
			if(i == j)
				aux[i][j] = 0;
			else 
				aux[i][j] = INT_MAX;
		}
	}
	int temp;

	for(int len=2; len<=mat_size; ++len) {
		//#pragma omp parallel default(shared)
		//{
		#pragma omp parallel num_threads(2)
		{
		for(int i=0; i<mat_size-len+1; ++i) {
			if(i%omp_get_num_threads() == omp_get_thread_num()) {
				temp = aux[i][i+len-1];
				#pragma omp parallel for reduction(min:temp)
				for(int j=i+1; j<i+len; ++j) {
					temp = min(temp, aux[i][j-1] + aux[j][i+len-1] + (mat_dim[i].rows * mat_dim[j-1].cols * mat_dim[i+len-1].cols));
				}
				aux[i][i+len-1] = temp;
			}
			#pragma omp barrier
		}
		}
	}
		//#pragma omp barrier
		//}
	printf("%d\n", aux[0][mat_size-1]);
	return 0;
}
