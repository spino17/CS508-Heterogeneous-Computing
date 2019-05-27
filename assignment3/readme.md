# CS508 - Introduction to Heterogeneous computing final submission
(1.jpg)
## About Code
Code is divided in three files
1. img2txt.txt - seperates a 3 channel image into channels and saves each in a file.
2. conv.cu - convolution operation and save each channel of the convoluted image in seperate file.
3. txt2img.txt - merge seperate channels into one image and the image as out.jpg.

### How to run

Input file is 1.jpg

Compile the file conv.cu
`nvcc conv.cu -o conv`
Run the file
`./conv`


The output image is saved as out.jpg

The kernel file has the following format-
rows(m) cols(n)
elem_row1_col1 elem_row1_col2 ... elem_row1_coln
.
.
.
.
.
.
elem_rowm_col1 elem_rowm_col2 ... elem_rowm_coln