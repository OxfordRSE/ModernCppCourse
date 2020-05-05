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

    // Compute mean
    double sum = 0.0;
    for (int i = 0; i < v.size(); i++)
    {
        sum += v[i];
    }
    const double mean = sum / v.size();

    // Compute standard deviation
    sum = 0.0;
    for (int i = 0; i < v.size(); i++)
    {
        sum += v[i] * v[i];
    }
    const double var = sum / v.size() - mean * mean;

    // Compute variance
    const double std = std::sqrt(var);

    std::cout << "mean:   " << mean << '\n';
    std::cout << "var:    " << var << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 3. Calculate the skewness - there's no default algorithm for that!
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Compute skewness
    sum = 0.0;
    double one_over_sigma3 = 1.0 / (std * std * std);
    for (int i = 0; i < v.size(); i++)
    {
        sum += (v[i] - mean) * (v[i] - mean) * (v[i] - mean) * one_over_sigma3;
    }
    const double skew = sum / v.size();

    std::cout << "skew:   " << skew << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 4. Calculate the median (and bonus chrono)
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto t1 = std::chrono::high_resolution_clock::now();

    // Take a copy of the vector so we can sort it
    std::vector<double> v2 = v;

    std::sort(v2.begin(), v2.end());
    int half_way = v2.size() / 2;
    const double median = v2[half_way];

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = t2 - t1;
    std::cout << std::fixed << "median took " << ms.count() << " ms\n";

    std::cout << "median: " << median << '\n';


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// 5. What other algorithms are there?
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Are any of the elements > 50?
    bool any_greater_than_50 = false;

    for (int i = 0; i < v.size(); i++)
    {
        if (v[i] > 50.0)
        {
            any_greater_than_50 = true;
        }
    }
    std::cout << std::boolalpha << "Any greater than 50? " << any_greater_than_50 << '\n';

    // First position where consecutive elements differ by more than twice the standard deviation
    int dist = 0;
    double first, second;
    for (int i = 0; i < v.size() - 1; i++)
    {
        if (std::fabs(v[i + 1] - v[i]) > 2 * std)
        {
            dist = i;
            first = v[i];
            second = v[i + 1];
            break;
        }
    }
    std::cout << "Position " << dist << ", first " << first << " second: " << second << '\n';


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
