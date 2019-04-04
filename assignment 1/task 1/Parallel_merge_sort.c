#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#define MAX_SIZE 100

/*
mpi program for merge sort 
*/

// loads the array from input file
int * load_array(const char * input_file_name, int * array_size) {
    FILE *f_input;
    int value, i;
    int * array = (int *)malloc(sizeof(int) * MAX_SIZE);
    f_input = fopen(input_file_name, "r");
    if (f_input == NULL) {
        printf("cannot open the file\n");
    } else {
        i = 0;
        // read the array from the file
        while(fscanf(f_input, "%d", &value) != EOF) {
            array[i] = value;
            i++;
        }
        *array_size = i; // stores the array size
    }
    fclose(f_input);
    return array;
}

// writes the array to the output file
void write_array(const char * output_file_name, int * array, int array_size) {
    int i = 0;
    FILE *f_output;
    f_output = fopen(output_file_name, "w");
    if (f_output == NULL) {
        printf("cannot open output file\n");
    } else {
        for(i = 0; i < array_size; i++) {
            fprintf(f_output, "%d\n", array[i]);
        }
    }
    fclose(f_output);
}

// merge function for merge sort
void merge(int * array, int start, int mid, int end) {
    int n1 = mid - start + 1;
    int n2 = end - mid;
    int l[n1], r[n2], i = 0, j = 0, k = 0;
    for(i = 0; i < n1; i++) {
        l[i] = array[start + i];
    }
    for(j = 0; j < n2; j++) {
        r[j] = array[mid + j + 1];
    }
    i = 0;
    j = 0;
    k = start;
    while (i < n1 && j < n2) {
        if (l[i] > r[j]) {
            array[k] = r[j];
            k++;
            j++;
        } else {
            array[k] = l[i];
            k++;
            i++;
        }
    }
    while (i < n1) {
        array[k] = l[i];
        k++;
        i++;
    }
    while (j < n2) {
        array[k] = r[j];
        k++;
        j++;
    }
}

// sort function for merge sort
void merge_sort(int * array, int start, int end) {
    if(start < end) {
        int mid = start + (end - start) / 2;
        merge_sort(array, start, mid);
        merge_sort(array, mid + 1, end);
        merge(array, start, mid, end);
    }
}

int main(int argc, char * argv[]) {
    int process_id, num_processes, array_size = 0, root = 0, i, send_count = 0;

    // mpi initialization
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int * recv_data = NULL;
    int * array = NULL;
    recv_data = (int *)malloc(sizeof(int) * MAX_SIZE); // to store respective part of the array
    if (process_id == root) {
        array = (int *)malloc(sizeof(int) * MAX_SIZE);
        array = load_array("input_array.txt", &array_size); // root processor loads the input array
    }
    MPI_Bcast(&array_size, 1, MPI_INT, root, MPI_COMM_WORLD);
    send_count = array_size / num_processes;
    MPI_Scatter(array, send_count, MPI_INT, recv_data, send_count, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD); // wait to receive the data
    int sub_array_size = send_count;
    for(i = 0; i < sub_array_size; i++) {
        //printf("%d, id: %d\n", recv_data[i], process_id);
    }
    // all the respective array are stored in recv_data
    merge_sort(recv_data, 0, sub_array_size - 1); // sort the respective array
    for(i = 0; i < sub_array_size; i++) {
        printf("%d, id: %d\n", recv_data[i], process_id);
    }
    // rest of the computation on all processors
/*
    if (process_id == root) {
        write_array("sorted_array.txt", sorted_array, array_size); // root processor writes the sorted array
    }
*/
    MPI_Finalize();

    return 0;
}
