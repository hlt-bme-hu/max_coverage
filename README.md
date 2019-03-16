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
            0 1
            0 2
            1 2
    The first line of the output is the maximum coverage per the size of the union of the sets.
    The rest of the output are the optimal solutions. Each line is a list of k indices.
    The indices determine which sets to choose.

## algorithm
Simply looks all the *n*-choose-*k* possibilities, where _k_ is the number of sets to choose and _n_ is the number of the sets to choose from (lines of input).

The key components of the algorithm are compile time optimized.
This bloats the size of the compiled binary and forces some limitations on the size of the sets and on the _k_ parameter.
But those limitations can be easily expanded.
The number of sets (_n_) is not limited compile time.

### parallelization
