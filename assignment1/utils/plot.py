import matplotlib.pyplot as plt
x = [1, 2, 4, 8]
y = [1.44, 0.03, 0.01, 0.1]

plt.xlabel('no. of threads')
plt.ylabel('time of execution')
plt.scatter(x, y)
plt.show()