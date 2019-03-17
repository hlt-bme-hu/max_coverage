# max_coverage
Calculates [maximum coverage problem](https://en.wikipedia.org/wiki/Maximum_coverage_problem)

Given a list of sets (over a given base set) the question is: which _k_ sets have to be chosen to cover the base set as much as possible.

The solution is one (or more) _k_-sized subset of the given sets.

## build

    cmake -DCMAKE_BUILD_TYPE=Release . && make

### requirements
* [cmake](https://cmake.org/)
* C++11 compilant compiler
* optionals for __distributed parallelization__
  * [gnu parallel](https://www.gnu.org/software/parallel/) 
  * python
  * bash 

## usage
See `maxcover --help`

    Calculates maximum coverage of sets.
    Author: Gabor Borbely, contact: borbely@math.bme.hu

    The input is read from stdin, one line represents a set, a 'set' is a list of words separated by spaces.
    Command line arguments:
            -k <int>        the exact number of sets you can choose. positive integer, default: 5
            -p --progress --print   prints progress periodically to stderr, default: false
            --file -i --input <filename>    default: "-"
            -b --begin -f --from <indices>  starting subset
            -e --end -t --to <indices>      ending subset, not inclusive

    Example:
            maxcover -k 2 < test/01.txt
    Output:
            4/6
            0	1
            0	2
            1	2
    The first line of the output is the maximum coverage per the size of the union of the sets.
    The rest of the output are the optimal solutions. Each line is a list of k indices.
    The indices determine which sets to choose.

If you interrupt the calculation (CTRL+C) then the so-far-best solution(s) are printed, which is not the guaranteed maximum.
But later you can continue the search (see parallelization).

## algorithm
Simply looks all the *n*-choose-*k* possibilities, where _k_ is the number of sets to choose and _n_ is the number of the sets to choose from (lines of input).

The key components of the algorithm are compile time optimized.
This bloats the size of the compiled binary and forces some limitations on the size of the sets and on the _k_ parameter.
But those limitations can be easily expanded.
The number of sets (_n_) is not limited compile time.

### parallelization
The parallelization is done in the least fancy way possible, however quite effective.

For example you have 10 sets and _k=5_ and you want to work on 8 threads.
The total 10-choose-5 = 252  possibilities can be split into 8 subsets.
For each subset of combinations you can calculate the maximum solutions, in parallel.
The final solution is the maximum of those 8 sub-solutions.

Since the combinations can be ordered lexicographically, one can represent a subset of combinations with an interval-like notation.
For example:

    "0 1 2 3 4" - "0 1 3 8 9"
    "0 1 3 8 9" - "0 2 3 4 7"
    "0 2 3 4 7" - "0 2 5 6 8"
    "0 2 5 6 8" - "0 3 4 5 6"
    "0 3 4 5 6" - "0 3 4 5 7"
    "0 3 4 5 7" - "2 3 4 8 9"
    "2 3 4 8 9" - "2 5 7 8 9"
    "2 5 7 8 9" - 
   
The last valid combination in this example is `"5 6 7 8 9"`.
In the example the size of the splits are quite uneven (the right end of the interval is exclusive):

    "0 1 2 3 4" - "0 1 3 8 9" ->  35
    "0 1 3 8 9" - "0 2 3 4 7" ->  23
    "0 2 3 4 7" - "0 2 5 6 8" ->  24
    "0 2 5 6 8" - "0 3 4 5 6" ->   9
    "0 3 4 5 6" - "0 3 4 5 7" ->   1
    "0 3 4 5 7" - "2 3 4 8 9" -> 113
    "2 3 4 8 9" - "2 5 7 8 9" ->  24
    "2 5 7 8 9" -             ->  23
    -------------------------------
    "0 1 2 3 4" -             -> 252

So in this case the subtask `"0 3 4 5 7" - "2 3 4 8 9"` is the bottleneck.
Nonetheless, if you choose these intervals then you can break down the search into subtasks, which can be calculated separately and remotely.
As you can see, the effectiveness of the parallelization relies on the **evenly split subtasks**.

A more even split would be:

    "0 1 2 3 4" - "0 1 6 7 8" -> 52
    "0 1 6 7 8" - "0 2 6 7 8" -> 35
    "0 2 6 7 8" - "0 6 7 8 9" -> 38
    "0 6 7 8 9" - "1 3 6 7 8" -> 52
    "1 3 6 7 8" - "1 6 7 8 9" -> 18
    "1 6 7 8 9" - "2 3 6 7 8" -> 17
    "2 3 6 7 8" - "2 6 7 8 9" -> 18
    "2 6 7 8 9" -             -> 22

The pseudo-code for parallelization:
1. break down the search space into intervals (use `generate_boundaries.py`)
1. compute all those using `test.sh`
1. find the maximum among the sub-tasks (with `post_process.sh`).

See `exp.sh` for example.
