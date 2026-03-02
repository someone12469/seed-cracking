This document describes the approach that was used to obtain the seed for the 1.21.4 panorama (-2132177964578008911, recorded in 24w40a at 3052 111 335, requires tp to `/locate biome pale_garden` before camera coordinates for precise match due to chunk generation order). It should be possible to apply it to very many other seeds (but it's computationally feasible only for the 2^48 random ones). A single screenshot containing a visible biome boundary is often sufficient for recovery.

Explanatory message copied from discord:
```
I was actually bruteforcing neither the terrain nor the trees
the method is very general, though it doesn't break the 1.21 or 1.17 panoramas and seems difficult for 26.1
the main target was the biome zoom seed, which equals sha256(seed) % 2^64 and is used to interpolate biomes from 4x4x4 quarts onto individual blocks
because of how the biome zoom logic is implemented, only 34 bits of the hash affect the result
so the overall scheme is
1) brute force the 2^34 functionally distinct biome zoom seeds
2) brute force the 2^48 random seeds to filter by SHA256 hash
3) filter resulting 2^14 seeds by checking the biomes
4) verify the last 4 or so by loading them and noticing that in one, even though our head is in a pale oak log, the terrain is suspiciously similar [this was written before people figured out that an older snapshot was used]

what makes brute forcing the biome zoom seed possible is observing biome data for individual blocks near a biome transition
this is possible to do directly in some cases like a screenshot of desert bordering plains -- because of surface rules the sand blocks precisely correspond to desert while the grass must be plains
here we do not see anything like this. but there is one more thing for which biome data is used: client-side grass, foliage and water coloring. different biomes have different colors
far in the background of one of the panorama's screenshots, we can see two pale garden to dark forest transitions, with a small patch of grass on the left and some leaves on the right
the displayed color is computed according to the biome blend video setting -- the biome-specific leaf/foliage/water colors are linearly averaged in a 5x5 (by default) flat area and the average is multiplied by the texture to obtain the tinted result. then brightness, smooth lighting etc. are applied
this means that very precisely measuring the color of leaves and grass can tell us how many blocks in a 5x5 range are in which biome
we can use this data to verify candidate biome zoom seeds
```
Another message:
```
- to verify a candidate biome seed, we also need to find the corresponding biomes, which we don't observe directly. I do this by brute force (with cutoffs if some guess clearly results in a contradiction)
- the biome quarts, except for caves, are equal within vertical lines. but the interpolation is not -- when moving vertically near a biome boundary, you can use f3 to observe that your current biome changes
- because of this, the leaves provide much better information than the grass -- imaginary scenario: n by n grass has us brute forcing (n/4)^2+O(n) biome states and verifying against n^2 horizontal blocks, while leaves have layers and so you get to verify against 3n^2
- additionally, in fancy graphics, leaves have holes. this means that we can observe some leaves through holes in other leaves to get more data
- the 5x5 blend complicates the analysis but probably makes it stronger overall as we get some information on a 5x5 area instead of just one block
- therefore observing the leaves attached to this message leaves just 559 biome seeds out of 2^34. the grass to the left of them gets something like 200k. combining them results in one (with <0.1 expected false positives, and zero in practice)
```

This branch contains most relevant code (which is not in a very clean state yet). The stages for cracking a seed using this method are roughly as follows. Code modifications are very likely to be required.

1. Construct a precise recreation of the target scene, including coordinates, block placement, perspective, window shape and video settings but not necessarily biomes.
2. Load the recreation with the test mod in the directory `mod`, the data pack `test.zip` and smooth lighting disabled. The data pack disables block textures and makes tintable blocks render using only their tint color. The mod replaces this tint color with the block's coordinates modulo 256. Take a screenshot. This screenshot will be used to map pixel coordinates of tintable blocks in the original image to world coordinates.
3. Modify the script `grass.py` or `foliage.py` to analyze the block tints at a visible biome transition. For each block, the goal is to derive the number of blocks in each biome in its 5x5 averaging area. This may require modifying the heuristics used to avoid errors due to a small number of pixels not matching between the original and the recreation (it may not be possible to achieve a precise match due to GPU rasterization being vendor-dependent). This script outputs lines of data in the format `x y z range min_biome1 max_biome1`, where `min_biome1` and `max_biome1` are bounds on the number of nearby blocks in one of the biomes.
4. Verify that this data appears correct. Pass this data to `zoombrute.cpp` (build with `-std=c++2c -O3 -fopenmp`), which will output a list of possible biome zoom seed lower34s for which, for some placement of biomes, can produce a boundary with the observed properties.
5. Modify `kernel.cl` and/or `hashbrute.cpp` to brute force the 2^48 random seeds to find those ~2^14 that have this biome zoom seed. This requires computing a SHA-256 hash for each seed, which is implemented with GPU support using OpenCL and takes 3-4 hours on a Nvidia RTX 5090 GPU. If the seed is not among those 2^48 which can be produced by the client if the seed box is left blank, this will fail; brute forcing 2^64 SHA-256 hashes is very difficult (which is to say, would take the Bitcoin network about 10 milliseconds).
6. Filter the ~2^14 (or more, if the code was modified to work with multiple lower34 candidates) seeds through some other method, like having the correct biomes near camera coordinates. Look at the results in-game.

Some information specific to the 1.21.4 panorama:

- The scripts `grass.py` and `foliage.py` reference the files `p4.png` (panorama image used for cracking) and `screenshot.png` (screenshot made with the mod and datapack); they are included in the repo.
- The file `oklist.txt` contains some older, less precise zoom seed filtering results, and contains the correct answer, which is 16074632720. It can be useful to speed up debugging.
- The recreation world download is in `world.zip`. The coordinates used for the analysis are `/tp @p 3052.723999770137 111.65018722695467 335.0234971379097 245.54265071122518`.
