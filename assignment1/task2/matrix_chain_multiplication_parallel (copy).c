#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <mpi.h>
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

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int this_rank, size, root_rank=0;
    MPI_Comm_rank(MPI_COMM_WORLD, &this_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int mat_size;
    Dimensions* mat_dim = getMatrices("inp.in", &mat_size);

    int *data = (int *)malloc(MAX_SIZE*MAX_SIZE*sizeof(int));
    int **aux = (int **)malloc(MAX_SIZE*sizeof(int*));

    for (int i=0; i<MAX_SIZE; i++)
        aux[i] = &(data[MAX_SIZE*i]);

    for(int i=0; i<MAX_SIZE; ++i){
        for(int j=0; j<MAX_SIZE; ++j){
            if(i == j)
                aux[i][j] = 0;
            else 
                aux[i][j] = INT_MAX;
        }
    }

    for(int len=2; len<=mat_size; ++len) {
        for(int i=0; i<mat_size-len+1; ++i) {
            if(this_rank == i) {
                for(int j=i+1; j<i+len; ++j) {
                    aux[i][i+len-1] = min(aux[i][i+len-1], aux[i][j-1] + aux[j][i+len-1] + (mat_dim[i].rows * mat_dim[j-1].cols * mat_dim[i+len-1].cols));
                }
                if(this_rank != root_rank) {
                    MPI_Send(&aux[i][i+len-1], 1, MPI_INT, 0, i, MPI_COMM_WORLD);
                }
                break;
            }
        }
        if(this_rank == root_rank) {
            for(int i=1; i<mat_size-len+1; ++i) {
                MPI_Recv(&aux[i][i+len-1], 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        MPI_Bcast(&aux[0][0], MAX_SIZE*MAX_SIZE, MPI_INT, root_rank, MPI_COMM_WORLD);
    }
    // printf("%d", min(50 ,10));
    // for(int i=0; i<mat_size; ++i){
    //     for(int j=0; j<MAX_SIZE; ++j){
    //         printf("%d ", aux[i][j]);
    //     }
    //     printf("%d %d\n", mat_dim[i].rows, mat_dim[i].cols);
    // }
    if(this_rank == root_rank)
        printf("%d\n", aux[0][mat_size-1]);
    MPI_Finalize();
    return 0;
}