# KnnPosix
KNN Algorithm using POSIX Threads

Implementation of the k-nearest-neighbors (k-nearest neighbors) `q` x-points belonging
in `d`-dimensional space (query) from a set of points `n` (data set). The program returns 
their k-nearest-neighbors for each query. Criterion for finding neighbors is the
Euclidean distance.

The program should work correctly for <img src="https://latex.codecogs.com/png.latex?{n<2^{20}}" alt="{mathcode}">, 
<img src="https://latex.codecogs.com/png.latex?{d\le128}" alt="{mathcode}">, 
<img src="https://latex.codecogs.com/png.latex?{k\le128}" alt="{mathcode}">, 
and 
<img src="https://latex.codecogs.com/png.latex?{q\le2^{10}}" alt="{mathcode}">, 
where `n` is the number of points in the original data set, `d` the number of dimensions, 
`k` the number of the requested neighbors and `q` the number of queries to the data set.
