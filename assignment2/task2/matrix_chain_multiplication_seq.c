#include <stdio.h>
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

    for(int len=2; len<=mat_size; ++len) {
        for(int i=0; i<mat_size-len+1; ++i) {
            for(int j=i+1; j<i+len; ++j) {
                aux[i][i+len-1] = min(aux[i][i+len-1], aux[i][j-1] + aux[j][i+len-1] + (mat_dim[i].rows * mat_dim[j-1].cols * mat_dim[i+len-1].cols));
            }
        }
    }
    // printf("%d", min(50 ,10));
    // for(int i=0; i<mat_size; ++i){
    //     for(int j=0; j<MAX_SIZE; ++j){
    //         printf("%d ", aux[i][j]);
    //     }
    //     printf("%d %d\n", mat_dim[i].rows, mat_dim[i].cols);
    // }
    printf("%d\n", aux[0][mat_size-1]);
    return 0;
}