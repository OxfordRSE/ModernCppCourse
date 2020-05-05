#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

namespace fs = std::filesystem;

int main()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 1. Reading some data in from a file
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Find all files like *_rse_workshop.dat under a specific directory
    std::vector<fs::path> data_files;
    for (auto &p : fs::recursive_directory_iterator(std::getenv("HOME")))
    {
        if (p.path().string().ends_with("_rse_workshop.dat"))
        {
            data_files.emplace_back(p.path());
        }
    }

    // This is just to make sure the same file is first, so the numbers later are the same on each version.
    // The sort isn't necessary!
    std::sort(data_files.begin(), data_files.end());

    // Which data files did we find? (range-for loop)
    for (const auto &d : data_files)
    {
        std::cout << d << std::endl;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 2. Calculate the mean and standard deviation of some data
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Read some data in
    std::ifstream f(data_files[0]);
    std::vector<double> v{std::istream_iterator<double>(f), std::istream_iterator<double>()};

    const double mean = std::reduce(v.begin(), v.end(), 0.0) / v.size();
    const double var = std::transform_reduce(v.begin(), v.end(), v.begin(), 0.0) / v.size() - mean * mean;
    const double std = std::sqrt(var);

    std::cout << "mean:   " << mean << '\n';
    std::cout << "var:    " << var << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 3. Calculate the skewness - there's no default algorithm for that!
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Compute skewness
    auto third_moment = [mean, std](const double x) { return std::pow((x - mean) / std, 3); };
    const double skew = std::transform_reduce(v.begin(), v.end(), 0.0, std::plus<>(), third_moment) / v.size();

    std::cout << "skew:   " << skew << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 4. Calculate the median (and bonus chrono)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto t1 = std::chrono::high_resolution_clock::now();

    // Take a copy of the vector so we can sort it
    std::vector<double> v2 = v;

    auto half_way = v2.size() / 2;
    std::nth_element(v2.begin(), std::next(v2.begin(), half_way), v2.end());
    const double median = v2.at(half_way);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = t2 - t1;
    std::cout << std::fixed << "median took " << ms.count() << " ms\n";

    std::cout << "median: " << median << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 5. What other algorithms are there?
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Any of the elements > 50?
    const bool any_greater_than_50 = std::any_of(v.begin(), v.end(), [](const double x) { return x > 50.0; });
    std::cout << std::boolalpha << "Any greater than 50? " << any_greater_than_50 << '\n';

    // First position where consecutive elements differ by more than twice the standard deviation
    auto comparison = [std](const double x, const double y) { return std::fabs(x - y) < 2.0 * std; };
    const auto answer = std::mismatch(v.begin(), std::prev(v.end()), std::next(v.begin()), comparison);

    const auto dist = std::distance(v.begin(), answer.first);
    std::cout << "Position " << dist << ", first " << *answer.first << " second: " << *answer.second << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 7. Writing some data back out to a csv file - pulling several concepts together
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string output_dir = "./output";
    std::string mkdir_command = "mkdir -p " + output_dir;
    // System takes a const char* as a parameter
    system(mkdir_command.c_str());

    // Create an output file
    std::ofstream output_file(output_dir + "/results.dat");
    output_file << v[0];
    for (int i = 1; i < v.size(); i++)
    {
        output_file << ',' << v[i];
    }
    output_file.close();
}
