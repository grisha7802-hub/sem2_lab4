#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "ArraySequence.hpp"
#include "OnlineStatistics.hpp"
#include "Stream.hpp"

static int parseInt(const std::string& value) {
    return std::stoi(value);
}

template<class Func>
static long long MeasureMilliseconds(Func action) {
    auto start = std::chrono::high_resolution_clock::now();
    action();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    std::cout << "case,size,metric,value,ms\n";

    int seedRaw[1] = {0};
    ArraySequence<int> seed(seedRaw, 1);
    auto naturals = Stream<int>::Recurrence(
        seed,
        [](const Sequence<int>& prefix) {
            return prefix.GetSize();
        },
        SequenceLength::Infinity()
    );

    long long lazySum = 0;
    long long lazyMs = MeasureMilliseconds([&] {
        auto firstMillion = naturals.Take(1000000);
        for (int index = 0; index < firstMillion.GetSize(); ++index) {
            lazySum += firstMillion.Get(index);
        }
    });
    std::cout << "lazy_take,1000000,sum," << lazySum << "," << lazyMs << "\n";

    const char* fileName = "lr4_bench_numbers.tmp";
    long long writeMs = MeasureMilliseconds([&] {
        std::ofstream output(fileName);
        for (int index = 0; index < 200000; ++index) {
            output << index << '\n';
        }
    });
    std::cout << "file_write,200000,items,200000," << writeMs << "\n";

    long long fileSum = 0;
    long long readMs = MeasureMilliseconds([&] {
        ReadOnlyStream<int> stream(fileName, parseInt);
        stream.Open();
        while (!stream.IsEndOfStream()) {
            fileSum += stream.Read();
        }
        stream.Close();
    });
    std::remove(fileName);
    std::cout << "file_read,200000,sum," << fileSum << "," << readMs << "\n";

    ReadOnlyStream<int> statsStream(std::string("8 1 5 3 13 21 2 34 55 1"), parseInt);
    OnlineStatistics<int> stats;
    long long statsMs = MeasureMilliseconds([&] {
        stats.Consume(statsStream);
    });
    std::cout << "online_stats," << stats.GetCount() << ",mean," << stats.GetMean() << "," << statsMs << "\n";
    std::cout << "online_stats," << stats.GetCount() << ",median," << stats.GetMedian() << "," << statsMs << "\n";
    std::cout << "online_stats," << stats.GetCount() << ",window3mean," << stats.GetWindowMean(3) << "," << statsMs << "\n";

    return 0;
}
