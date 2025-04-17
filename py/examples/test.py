from pyalgo import PointSource, TriangleSource
from collections import OrderedDict

import numpy as np
import matplotlib.pyplot as plt


def plot(Z: list[list[float]], n: int, name: str):
    min = np.min(Z)
    max = np.max(Z)
    # print('min, max =', min, max)
    if min == max:
        return

    coords = np.linspace(-2, 2, n)
    levels = np.linspace(min, max, n)
    X, Y = np.meshgrid(levels, coords)
    
    cmap = 'jet'
    
    fig, ax = plt.subplots(figsize=(6, 6))
    CS = ax.contourf(coords, coords, Z, levels=levels, cmap=cmap)
    CS2 = ax.contour(CS, levels=CS.levels[::3], colors='black', linewidths=1)

    ax.margins(0.2)
    ax.set_xlabel('x')
    ax.set_ylabel('y')
    ax.set_title(name)
    
    cbar = fig.colorbar(CS)
    cbar.add_lines(CS2)

    plt.show()

a = PointSource([0,0,0], [1, 2, 0])
b = TriangleSource([0,0,0], [1,0,0], [0,1,0], [1,0,-1], 1, 0.25, 8)

# -----------------------------------------------------

min = -5
max =  5
n   = 51

d = OrderedDict([(0, 'Sxx'), (1, 'Sxy'), (2, 'Sxz'), (3, 'Syy'), (4, 'Syz'), (5, 'Szz')])

for index in range(0, 6):
    Z = np.zeros( shape = (n,n) )
    for i in range(0, n):
        x = min + i/(n-1)*(max-min)
        for j in range(0, n):
            y = min + j/(n-1)*(max-min)
            Z[i][j] = b.stresses([x, y, 1])[index] # 0: Sxx etc...
    plot(Z, n, d[index])
