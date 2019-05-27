import cv2
import numpy as np
import sys
import os
import cv2
ch1 = np.loadtxt('ch1_out.txt', delimiter='\n').astype(int)
ch2 = np.loadtxt('ch2_out.txt', delimiter='\n').astype(int)
ch3 = np.loadtxt('ch3_out.txt', delimiter='\n').astype(int)
h,w = ch1[0], ch1[1]
ch1 = np.reshape(ch1[2:], (h,w))
ch2 = np.reshape(ch2, (h,w))
ch3 = np.reshape(ch3, (h,w))
# print(ch1)
img = np.zeros([h,w,3])

img[:,:,0] = ch1
img[:,:,1] = ch2
img[:,:,2] = ch3

cv2.imwrite('out.jpg', img)