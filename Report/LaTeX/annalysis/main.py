from cProfile import label
import matplotlib.pyplot as plt
import numpy as np
x = np.array([1000, 2000, 4000, 5000])
y = np.array([0.332, 0.284, 0.227, 0.266])
y = np.array([0.238, 0.227, 0.224, 2.328])

plot1 = plt.plot(x,y,'bo--',label='Time Per Request')
plt.title('c:1000, n:[1000~5000]')
plt.xlabel('n')
plt.ylabel('Time per Request')

plt.show()

