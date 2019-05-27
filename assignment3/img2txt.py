import cv2
import numpy as np
import sys

file = "1.jpg"

img = cv2.imread(file)
h,w,n = img.shape
ch1 = img[:,:,0]
ch2 = img[:,:,1]
ch3 = img[:,:,2]
ch1 = np.reshape( ch1, ( h*w ,  ) )
ch2 = np.reshape( ch2, ( h*w ,  ) )
ch3 = np.reshape( ch3, ( h*w ,  ) )
ch1 = np.append(np.array( [h,w] ) , ch1 ).astype(int)

np.savetxt('ch1.txt', ch1, delimiter='\n',fmt='%1d') 
np.savetxt('ch2.txt', ch2, delimiter='\n',fmt='%1d') 
np.savetxt('ch3.txt', ch3, delimiter='\n',fmt='%1d') 
