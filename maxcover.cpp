#include <iostream>
#include <sstream>
#include <fstream>

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <bitset>
#include <array>

#include "utils.h"
#include "stopsignal.h"

extern "C" volatile int running;

typedef std::unordered_set<std::string> Set;

struct SetHash
{
    size_t operator()(const Set& set)const
    {
        static const auto hasher = std::hash<std::string>();
        size_t result = 0;
        for (const auto& elem : set)
            result ^= hasher(elem);
        return result;
    }
};

template<size_t n_entities>
std::vector<std::bitset<n_entities>> BitSetsFromSets(
        const std::vector<Set>& sets,
        const std::unordered_map<std::string, size_t>& names)
{
    std::bitset<n_entities> x;
    std::vector<std::bitset<n_entities>> binary_sets;
    binary_sets.reserve(sets.size());
    for (const auto& set : sets)
    {
        x.reset();
        for (const auto& elem : set)
            x[names.at(elem)] = true;
        binary_sets.emplace_back(x);
    }
    return binary_sets;
}

template<size_t n_entities, class Collection>
size_t sets_union(const std::vector<std::bitset<n_entities>>& binary_sets, const Collection& c)
{
    auto x = binary_sets[c[0]];
    for (size_t i = 1; i < c.size(); ++i)
        x |= binary_sets[c[i]];
    return x.count();
}

template<size_t n_entities, int k>
void get_max_cover_static2(const std::vector<Set>& sets, const std::unordered_map<std::string, size_t>& names,
    const std::vector<size_t>& _begin, const std::vector<size_t>& _end, bool print_progress = false)
{
    const std::vector<std::bitset<n_entities>> binary_sets = BitSetsFromSets<n_entities>(sets, names);
    const auto n = sets.size();
    const auto N = nchoosek(n, k);
    
    typedef std::array<size_t, k> collection;
    
    collection indices, end;

    for (size_t i = 0; i < k; ++i)
    {
        indices[i] = _begin[i];
        end[i] = _end[i];
    }

    std::pair<size_t, std::vector<collection>> best;
    best.first = 0; best.second.clear();
    size_t this_count = 0;
    size_t progress = 0;
    ProgressIndicator("\r%zu/%zu @ %zu/%zu", print_progress, [&]()
    {
        int i;
        int b = 0; // first index where 'indices' and 'end' differ
        for (; b < k; ++b)
            if (indices[b] != end[b])
                break;

        // https://docs.python.org/2/library/itertools.html#itertools.combinations
        while (running && b < k)
        {
            //for (auto x : indices)
            //    std::cerr << x << '\t';
            //std::cerr << std::endl;

            this_count = sets_union(binary_sets, indices);
            if (this_count > best.first)
            {
                best.second.clear();
                best.second.push_back(indices);
                best.first = this_count;
            }
            else if (this_count == best.first)
            {
                best.second.push_back(indices);
            }
            ++progress;

            for (i = k - 1; i >= b; --i)
                if (indices[i] != (i + n) - k)
                    break;

            if (i < b)
                break;

            indices[i] += 1;
            for (size_t j = i + 1; j < k; ++j)
                indices[j] = indices[j - 1] + 1;
            if (i == b) // recalculates place of first differ
                for (; b < k; ++b)
                    if (indices[b] != end[b])
                        break;
        }
    }, best.first, names.size(), progress, N);
    
    std::cout << best.first << '/' << names.size() << std::endl;
    for (const auto& collection : best.second)
    {
        std::cout << collection[0];
        for (size_t j = 1; j < k; ++j)
        {
            std::cout << '\t' << collection[j];
        }
        std::cout << '\n';
    }
}

bool read_collection(const std::string& text, std::vector<size_t>& c, size_t k)
{
    std::set<size_t> result;
    std::istringstream iss(text);
    size_t i;
    while (iss >> i)
    {
        result.emplace(i);
    }
    if (result.size() != k)
        return false;
    c.assign(result.begin(), result.end());
    return true;
}

template<size_t n>
bool get_max_cover_static1(const std::vector<Set>& sets, const std::unordered_map<std::string, size_t>& names,
    const std::vector<size_t>& _begin, const std::vector<size_t>& _end, size_t k, bool print_progress = false)
{
    auto fn = get_max_cover_static2<n, 1>;

         if (k == 1) fn = get_max_cover_static2<n, 1>;
    else if (k == 2) fn = get_max_cover_static2<n, 2>;
    else if (k == 3) fn = get_max_cover_static2<n, 3>;
    else if (k == 4) fn = get_max_cover_static2<n, 4>;
    else if (k == 5) fn = get_max_cover_static2<n, 5>;
    else if (k == 6) fn = get_max_cover_static2<n, 6>;
    else if (k == 7) fn = get_max_cover_static2<n, 7>;
    else if (k == 8) fn = get_max_cover_static2<n, 8>;
    else if (k == 9) fn = get_max_cover_static2<n, 9>;
    else if (k == 10) fn = get_max_cover_static2<n, 10>;
    else
    {
        std::cerr << "size of the collection is too big: " << k << std::endl;
        return false;
    }
    
    fn(sets, names, _begin, _end, print_progress);

    return true;
}

bool get_max_cover(const std::vector<Set>& sets, const std::unordered_map<std::string, size_t>& names,
    const std::vector<size_t>& _begin, const std::vector<size_t>& _end, size_t k, bool print_progress = false)
{
    const auto n = names.size();

    auto fn = get_max_cover_static1<64>;

         if (n <= 64)  fn = get_max_cover_static1<64>;
    else if (n <= 128) fn = get_max_cover_static1<128>;
    else if (n <= 192) fn = get_max_cover_static1<192>;
    else if (n <= 256) fn = get_max_cover_static1<256>;
    else if (n <= 320) fn = get_max_cover_static1<320>;
    else if (n <= 384) fn = get_max_cover_static1<384>;
    else if (n <= 448) fn = get_max_cover_static1<448>;
    else if (n <= 512) fn = get_max_cover_static1<512>;
    else if (n <= 576) fn = get_max_cover_static1<576>;
    else if (n <= 640) fn = get_max_cover_static1<640>;
    else
    {
        std::cerr << "size of the base set is too big: " << n << std::endl;
        return false;
    }
    return fn(sets, names, _begin, _end, k, print_progress);
}

int main(int, const char* argv[])
{
    std::vector<Set> sets;
    std::unordered_map<std::string, size_t> names;
    const char* const program = argv[0];
    size_t k = 5;
    bool print_progress = false;
    std::vector<size_t> begin, end;
    std::string input_filename = "-";

    for (++argv; *argv; ++argv)
    {
        if (matches(*argv, "-h", "--help"))
        {
            std::cout << "Calculates maximum coverage of sets.\n"
                "Author: Gabor Borbely, contact: borbely@math.bme.hu\n\n"
                "The input is read from stdin, one line represents a set, a 'set' is a list of words separated by spaces.\n"
                "Command line arguments:\n"
                "\t-k <int>\tthe exact number of sets you can choose. positive integer, default: " << k << "\n"
                "\t-p --progress --print\tprints progress periodically to stderr, default: " << (print_progress ? "true" : "false") << "\n"
                "\t--file -i --input <filename>\tdefault: \"" << input_filename << "\"\n"
                "\t-b --begin -f --from <indices>\tstarting subset\n"
                "\t-e --end -t --to <indices>\tending subset, not inclusive\n"
                "\nExample:\n\t" << program << " -k 2 < test/01.txt\n"
                "Output:\n\t4/6\n\t0 1\n\t0 2\n\t1 2\n"
                "The first line of the output is the maximum coverage per the size of the union of the sets.\n"
                "The rest of the output are the optimal solutions. Each line is a list of k indices.\n"
                "The indices determine which sets to choose."
                << std::endl;
            return 0;
        }
        else if (matches(*argv, "-k") && *(argv + 1) && atoi(*(argv + 1)) > 0)
        {
            k = (size_t)atoi(*++argv);
        }
        else if (matches(*argv, "--file", "-i", "--input") && *(argv + 1))
        {
            input_filename = *++argv;
        }
        else if (matches(*argv, "-b", "--begin", "-f", "--from") && *(argv + 1))
        {
            if (!read_collection(*++argv, begin, k))
            {
                std::cerr << "Invalid collection to begin with: \"" << *argv << "\"!" << std::endl;
                return 1;
            }
        }
        else if (matches(*argv, "-e", "--end", "-t", "--to") && *(argv + 1))
        {
            if (!read_collection(*++argv, end, k))
            {
                std::cerr << "Invalid collection as end: \"" << *argv << "\"!" << std::endl;
                return 1;
            }
        }
        else if (matches(*argv, "-p", "--progress", "--print"))
        {
            print_progress = true;
        }
        else
            std::cerr << "Unknown parameter \"" << *argv << "\"!" << std::endl;
    }

    if (setup_stop_signal() != 0)
    {
        std::cerr << "Unable to set up stop signal!" << std::endl;
    }
    {
    std::ifstream ifs(input_filename);
    std::istream& input = (input_filename == "-") ? std::cin : ifs;

    std::string line;
    while (std::getline(input, line))
    {
        Set newset;
        Set duplicates;
        std::istringstream iss(line);
        std::string name;
        bool OK = true;
        while (iss >> name)
        {
            names.emplace(name, names.size());
            if (!newset.insert(name).second)
            {
                if (duplicates.insert(name).second)
                {
                    if (OK)
                        std::cerr << "duplicate(s) in set #" << sets.size() << ":";
                    std::cerr << ' ' << name;
                    OK = false;
                }
            }
        }
        if (!OK)
            std::cerr << std::endl;
        sets.push_back(newset);
    }
    }
    {
        std::unordered_set<Set, SetHash> sets2(sets.begin(), sets.end());
        if (sets2.size() < sets.size())
            std::cerr << sets.size() - sets2.size() << " sets are duplicate!" << std::endl;
    }
    if (sets.empty() || names.empty())
    {
        std::cerr << "Unable to read anything from \"" << input_filename << "\"!" << std::endl;
        return 1;
    }

    if (sets.size() < k)
    {
        std::cerr << "There are less sets then 'k': " << sets.size() << " < " << k << std::endl;
        k = sets.size();
        std::cerr << "Overriding k = " << k << std::endl;
    }

    if (begin.empty())
    {
        for (size_t i = 0; i < k; ++i)
            begin.emplace_back(i);
    }
    else
    {
        for (auto i : begin)
            if (i >= sets.size())
            {
                std::cerr << "collection to begin with contains out-of-bound index: " << i << "!" << std::endl;
                return 1;
            }
    }
    if (end.empty())
    {
        for (size_t i = sets.size() - k; i < sets.size(); ++i)
            end.emplace_back(i);
        end.back() = sets.size();
    }
    else
    {
        for (size_t i = 0; i < k-1; ++i)
            if (end[i] >= sets.size()-1)
            {
                std::cerr << "end collection contains out-of-bound index: " << end[i] << "!" << std::endl;
                return 1;
            }
        if (end.back() > sets.size())
        {
            std::cerr << "end collection contains out-of-bound index: " << end.back() << "!" << std::endl;
            return 1;
        }
    }
    if (end < begin)
    {
        std::cerr << "'end' should be after 'begin'!" << std::endl;
        return 1;
    }
    return get_max_cover(sets, names, begin, end, k, print_progress) ? 0 : 1;
}
