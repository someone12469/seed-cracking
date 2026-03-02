import math
from collections import defaultdict, Counter
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
img = np.array(Image.open('p4.png'))[:,:,:3]
i2 = np.array(Image.open('screenshot.png'))[:,:,:3]
col0 = (0x77, 0x82, 0x72) # grass
col1 = (0x50, 0x7a, 0x32) # grass
#col0 = (0x87, 0x8d, 0x76) # foliage
#col1 = (0x59, 0xae, 0x30) # foliage
guess = {}
for i in range(0, 26):
    c = [(x*i + y*(25-i)) // 25 for x,y in zip(col0, col1)]
    def get(mul):
        return tuple([round(x * mul) for x in c])
    possible = set()
    def bisect(l, r, depth = 0):
        if depth >= 30: # enough
            return
        a = get(l)
        b = get(r)
        possible.add(a)
        possible.add(b)
        if a != b:
            bisect(l, (l+r)/2, depth + 1)
            bisect((l+r)/2, r, depth + 1)
    bisect(0, 1)
    for x in possible:
        if x not in guess:
            guess[x] = []
        guess[x].append(i)
vals = defaultdict(list)
for i in range(img.shape[0]):
    for j in range(img.shape[1]):
        if i < 510 or i > 550:
            continue
        if j < 480 or j > 520:
            continue
        c = tuple(img[i,j].tolist())
        if c in guess and -1 not in guess[c]:
            img[i, j] = (10 * guess[c][0], 255, 0) if len(guess[c]) == 1 else (255, 0, 0)
            if i2[i,j].tolist() == [255,255,255]:
                img[i, j] = (255, 0, 255)
            else:
                x,y,z = i2[i, j]
                x = int(x) + 256 * 11
                y = int(y)
                z = int(z) + 256 * 1
                print(x, y, z, guess[c])
                vals[x,y,z].append(guess[c])
for coord,g in vals.items():
    #print(coord, g)
    g = [set(ar) for ar in g]
    a0 = set(g[0])
    a1 = set(g[0])
    for x in g:
        a0 = a0 & x
        a1 = a1 | x
    scores = {v:0 for v in a1}
    for x in g:
        for el in a1:
            scores[el] += 1 if el in x else -2
    ssc = sorted(scores.items(), key = lambda p:-p[1])
    if len(g) > 0 and (len(ssc) == 1 or ssc[0][1] - ssc[1][1] >= 1):
        print(*coord, 5, ssc[0][0], ssc[0][0])
plt.imsave('map2.png', img)
exit()
