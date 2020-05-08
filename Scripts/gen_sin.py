import numpy as np
import matplotlib.pyplot as plt

STEPS = 256
PERIOD = 256
AMPLITUDE = 127

x = np.arange(0, STEPS)
func = lambda x, offset: np.floor((np.sin((2*np.pi *x/PERIOD) + offset) * AMPLITUDE))
y1 = func(x, 0)
y2 = func(x, 2/3*np.pi)
y3 = func(x, 4/3*np.pi)

y = np.vstack([y1, y2, y3])
ymin = np.amin(y, axis=0)
ymax = np.amax(y, axis=0)
yoffset = ((ymin + ymax)/2) - AMPLITUDE

y[0] -= yoffset
y[1] -= yoffset
y[2] -= yoffset

print(list(map(int, y[0].tolist())))

plt.plot(x, y[0])
plt.plot(x, y[1])
plt.plot(x, y[2])
plt.show()
