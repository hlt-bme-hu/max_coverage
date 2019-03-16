from __future__ import print_function
import numpy
import sys

def nchoosek(n, k):
    result = 1
    for i in range(n, n-k, -1):
        result *= i
    for i in range(2, k+1):
        result /= i
    return result
    
n, k, t = map(int, sys.argv[1:4])

boundaries = set()
boundaries.add(tuple(range(k)))
boundaries.add(tuple(range(n-k, n-1)) + (n,))

if t <= nchoosek(n, k):
    while len(boundaries) <= t:
        boundaries.add(tuple(sorted(numpy.random.choice(range(n), size=k, replace=False))))

for b in sorted(boundaries):
        print(' '.join(map(str, b)))

