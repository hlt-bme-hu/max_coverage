from __future__ import print_function
import sys
import numpy

def nchoosek(n, k):
    result = 1
    for i in range(n, n-k, -1):
        result *= i
    for i in range(2, k+1):
        result /= i
    return result
    
def search(x, goal, n, k):
          #0  0    10  1
    assert(0 <=k and k <=n)
    s = 0
    # if goal >= nchoosek(n, k):
        # return tuple(range(x, x+k-1)) + (x+k,)
    if k > 0:
        while x <= n-k:
            new_sum = s + nchoosek(n-x-1,k-1)
            if new_sum > goal:
                return (x,) + search(x+1, goal-s, n-1, k-1)
            else:
                s = new_sum
                x += 1

    return tuple(range(x, x+k))

def str_subset(x):
    return ' '.join(map(str, x))
    
if __name__ == "__main__":
    n, k, t = map(int, sys.argv[1:4])

    print(str_subset(range(k)))

    N = nchoosek(n, k)

    x = 0
    s = 0
    for goal in numpy.arange(float(N)/t, N, float(N)/t):
        print(str_subset(search(0, goal, n, k)))

    print(str_subset(tuple(range(n-k,n-1)) + (n,)))
