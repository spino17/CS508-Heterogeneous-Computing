#include <iostream>
#include <fstream>
#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>
#include "cuPrintf.cu"
using namespace std;
struct row_cols {
	int rows, cols;
};

typedef struct row_cols rc;

__global__ void conv_operate(int * d_arr1, int * d_arr2, int * d_arr3, int * d_kernel, int * d_conv_arr1, int * d_conv_arr2, int * d_conv_arr3, rc arr_size, rc kernel_size, rc conv_size) {
	int index = blockDim.x * blockIdx.x + threadIdx.x;
	int row_num = index/blockDim.x;
	int col_num = index%blockDim.x;
	int min_row = row_num;
	int max_row = row_num + kernel_size.rows-1;
	int min_col = col_num;
	int max_col = col_num + kernel_size.cols-1;
	int conv_val1=0, conv_val2=0, conv_val3=0;
	for (int i = min_row, conv_row=0; i <= max_row; i++, conv_row++) {
		for (int j = min_col, conv_col=0; j <= max_col; ++j, conv_col++) {
			conv_val1 += d_arr1[i*arr_size.cols + j] * d_kernel[conv_row*kernel_size.cols + conv_col];
			conv_val2 += d_arr2[i*arr_size.cols + j] * d_kernel[conv_row*kernel_size.cols + conv_col];
			conv_val3 += d_arr3[i*arr_size.cols + j] * d_kernel[conv_row*kernel_size.cols + conv_col];
		}
	}
	__syncthreads();
	atomicAdd((int*)&d_conv_arr1[index], (int)conv_val1/9);
	atomicAdd((int*)&d_conv_arr2[index], (int)conv_val2/9);
	atomicAdd((int*)&d_conv_arr3[index], (int)conv_val3/9);
}

int main(int argc, char *argv[]) {
	// Run the python script which converts image to seperate channels
	system("python img2txt.py");

	//Load the image which was seperated by python into different channels, each in its respective file.

	rc arr_size, kernel_size;
	fstream ch1("ch1.txt", std::ios_base::in);
	fstream ch2("ch2.txt", std::ios_base::in);
	fstream ch3("ch3.txt", std::ios_base::in);
    
    ch1 >> arr_size.rows >> arr_size.cols;
	int* channels[3];
	channels[0] = (int*) malloc(sizeof(int) * arr_size.rows * arr_size.cols);
	channels[1] = (int*) malloc(sizeof(int) * arr_size.rows * arr_size.cols);
	channels[2] = (int*) malloc(sizeof(int) * arr_size.rows * arr_size.cols);
	for (int i = 0; i < arr_size.rows * arr_size.cols; ++i) {
		ch1 >> channels[0][i];
		ch2 >> channels[1][i];
		ch3 >> channels[2][i];
	}
	cout << "Image loaded\n";
	
	//Load the kernel
	fstream kern("kernel_edge.txt", std::ios_base::in);
	kern >> kernel_size.rows;
	kern >> kernel_size.cols;
	
	int* kernel = (int*)malloc(sizeof(int) * (kernel_size.rows) * (kernel_size.cols));
	for (int i = 0; i < kernel_size.rows * kernel_size.cols; ++i) {
		kern >> kernel[i];
	}
	cout << "Kernel loaded\n";
	rc conv_size;
	conv_size.rows = (arr_size.rows) - (kernel_size.rows)+1;
	conv_size.cols = (arr_size.cols) - (kernel_size.cols)+1;

	// setup variables used in device
	int *d_arr1;
	int *d_arr2;
	int *d_arr3;
	int *d_kernel;
	int *d_conv_arr1;
	int *d_conv_arr2;
	int *d_conv_arr3;

	cudaMalloc((void **)&d_arr1, sizeof(int)  * (arr_size.rows) * (arr_size.cols));
	cudaMalloc((void **)&d_arr2, sizeof(int)  * (arr_size.rows) * (arr_size.cols));
	cudaMalloc((void **)&d_arr3, sizeof(int)  * (arr_size.rows) * (arr_size.cols));
	cudaMalloc((void **)&d_kernel, sizeof(int) * (kernel_size.rows) * (kernel_size.cols));
	cudaMalloc((void **)&d_conv_arr1, sizeof(int) * (conv_size.rows) * (conv_size.cols));
	cudaMalloc((void **)&d_conv_arr2, sizeof(int) * (conv_size.rows) * (conv_size.cols));
	cudaMalloc((void **)&d_conv_arr3, sizeof(int) * (conv_size.rows) * (conv_size.cols));

	cudaMemcpy(d_arr1, channels[0], sizeof(int) * (arr_size.rows) * (arr_size.cols), cudaMemcpyHostToDevice);
	cudaMemcpy(d_arr2, channels[1], sizeof(int) * (arr_size.rows) * (arr_size.cols), cudaMemcpyHostToDevice);
	cudaMemcpy(d_arr3, channels[2], sizeof(int) * (arr_size.rows) * (arr_size.cols), cudaMemcpyHostToDevice);
	cudaMemcpy(d_kernel, kernel, sizeof(int) * (kernel_size.rows) * (kernel_size.cols), cudaMemcpyHostToDevice);
	cudaMemset(d_conv_arr1, 0, sizeof(int) * (conv_size.rows) * (conv_size.cols));
	cudaMemset(d_conv_arr2, 0, sizeof(int) * (conv_size.rows) * (conv_size.cols));
	cudaMemset(d_conv_arr3, 0, sizeof(int) * (conv_size.rows) * (conv_size.cols));

	// // printf("mem copied\n");
	cout << "Processing\n";
	cudaPrintfInit();
	conv_operate<<<conv_size.rows,conv_size.cols>>>(d_arr1, d_arr2, d_arr3, d_kernel, d_conv_arr1, d_conv_arr2, d_conv_arr3, arr_size, kernel_size, conv_size);
	cudaPrintfDisplay(stdout, true);
	cudaPrintfEnd();
	
	free(channels[0]);
	free(channels[1]);
	free(channels[2]);
	free(kernel);

	//get the computed data back
	channels[0] = (int*) malloc(sizeof(int) * conv_size.rows*conv_size.cols);
	channels[1] = (int*) malloc(sizeof(int) * conv_size.rows*conv_size.cols);
	channels[2] = (int*) malloc(sizeof(int) * conv_size.rows*conv_size.cols);

	cudaMemcpy(channels[0], d_conv_arr1, sizeof(int) * (conv_size.rows) * (conv_size.cols), cudaMemcpyDeviceToHost);
	cudaMemcpy(channels[1], d_conv_arr2, sizeof(int) * (conv_size.rows) * (conv_size.cols), cudaMemcpyDeviceToHost);
	cudaMemcpy(channels[2], d_conv_arr3, sizeof(int) * (conv_size.rows) * (conv_size.cols), cudaMemcpyDeviceToHost);
	
	
	//write the data into seperate text files for each channel
	ofstream ch1_out("ch1_out.txt");
	ofstream ch2_out("ch2_out.txt");
	ofstream ch3_out("ch3_out.txt");
	int i=0;
	ch1_out << conv_size.rows << "\n" << conv_size.cols << "\n";
	for (; i < conv_size.rows * conv_size.cols - 1; ++i) {
		ch1_out << channels[0][i] << "\n";
		ch2_out << channels[1][i] << "\n";
		ch3_out << channels[2][i] << "\n";
	}
	ch1_out << channels[0][i];
	ch2_out << channels[1][i];
	ch3_out << channels[2][i];
	ch1_out.close();
	ch2_out.close();
	ch3_out.close();
	//Run python code which converts the seperated channels back to image
	system("python txt2img.py");
	cout << "Image Saved as out.jpg\n";
	return 0;
}