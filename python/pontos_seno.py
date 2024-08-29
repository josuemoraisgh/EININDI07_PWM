import math
import numpy as np

step = (math.pi/2)/255
print([round(math.sin(h)*255) for h in np.arange(0, math.pi/2+step, step)])