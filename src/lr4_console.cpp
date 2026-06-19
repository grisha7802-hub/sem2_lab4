#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ArraySequence.hpp"
#include "OnlineStatistics.hpp"
#include "Stream.hpp"

static int squareInt(int x) { return x * x; }
static bool isEvenInt(int x) { return x % 2 == 0; }
static int addInt(int a, int b) { return a + b; }
static int parseInt(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (...) {
        throw std::invalid_argument("parse int");
    }
}

static void PrintSequence(const Sequence<int>& values, const char* title) {
    std::cout << title << ": [";
    for (int index = 0; index < values.GetSize(); ++index) {
        std::cout << values.Get(index);
        if (index + 1 < values.GetSize()) std::cout << ", ";
    }
    std::cout << "]\n";
}

static ArraySequence<int> ReadManualSequence() {
    std::cout << "Enter integer line: ";
    std::cin.ignore(10000, '\n');
    std::string line;
    std::getline(std::cin, line);
    ReadOnlyStream<int> input(line, parseInt);
    ArraySequence<int> values;
    while (!input.IsEndOfStream()) {
        values.Append(input.Read());
    }
    return values;
}

int RunLr4Console() {
    int fibSeedRaw[2] = {0, 1};
    ArraySequence<int> fibSeed(fibSeedRaw, 2);
    auto fib = Stream<int>::Recurrence(
        fibSeed,
        [](const Sequence<int>& prefix) {
            int n = prefix.GetSize();
            return prefix.Get(n - 1) + prefix.Get(n - 2);
        },
        SequenceLength::Infinity()
    );

    int finiteRaw[8] = {3, 1, 4, 1, 5, 9, 2, 6};
    auto finite = Stream<int>::FromArray(finiteRaw, 8);

    while (true) {
        std::cout << "\nLR4 menu\n"
                  << "1 Show first N Fibonacci\n"
                  << "2 Show squares of first N Fibonacci\n"
                  << "3 Show even numbers from finite stream\n"
                  << "4 Manual ReadOnlyStream from line\n"
                  << "5 Manual WriteOnlyStream to sequence\n"
                  << "6 Online statistics for manual stream\n"
                  << "7 Manual Stream operation (Map/Where/Reduce/Zip/Stats)\n"
                  << "0 Exit\n";
        int cmd = 0;
        std::cin >> cmd;
        if (cmd == 0) break;

        if (cmd == 1) {
            int n = 0;
            std::cin >> n;
            PrintSequence(fib.Take(n), "Fibonacci");
        } else if (cmd == 2) {
            int n = 0;
            std::cin >> n;
            auto sq = fib.Map<int>(squareInt);
            PrintSequence(sq.Take(n), "Fib squares");
        } else if (cmd == 3) {
            auto evens = finite.Where(isEvenInt);
            PrintSequence(evens.Take(3), "Evens");
        } else if (cmd == 4) {
            auto values = ReadManualSequence();
            PrintSequence(values, "Read");
        } else if (cmd == 5) {
            std::cout << "Enter count and then values: ";
            int count = 0;
            std::cin >> count;
            ArraySequence<int> written;
            WriteOnlyStream<int> writer(&written);
            for (int index = 0; index < count; ++index) {
                int value = 0;
                std::cin >> value;
                writer.Write(value);
            }
            PrintSequence(written, "Written sequence");
        } else if (cmd == 6) {
            auto values = ReadManualSequence();
            ReadOnlyStream<int> input(values);
            OnlineStatistics<int> stats;
            stats.Consume(input);
            std::cout << "count=" << stats.GetCount()
                      << " sum=" << stats.GetSum()
                      << " min=" << stats.GetMin()
                      << " max=" << stats.GetMax()
                      << " mean=" << stats.GetMean()
                      << " median=" << stats.GetMedian()
                      << "\n";
        } else if (cmd == 7) {
            auto values = ReadManualSequence();
            auto stream = Stream<int>::FromSequence(values);
            std::cout << "Operation: 1 Map(^2), 2 Where(even), 3 Reduce(sum), 4 Zip with same stream, 5 Stats\n";
            int operation = 0;
            std::cin >> operation;
            if (operation == 1) {
                auto mapped = stream.Map<int>(squareInt);
                PrintSequence(mapped.Take(values.GetSize()), "Map result");
            } else if (operation == 2) {
                auto filtered = stream.Where(isEvenInt);
                PrintSequence(filtered.Take(filtered.Source().GetLength().IsFinite()
                    ? static_cast<int>(filtered.Source().GetLength().Value()) : values.GetSize()), "Where result");
            } else if (operation == 3) {
                std::cout << "Reduce sum = " << stream.Reduce<int>(addInt, 0) << "\n";
            } else if (operation == 4) {
                auto zipped = stream.Zip(stream);
                auto pairs = zipped.Take(values.GetSize());
                std::cout << "Zip result: [";
                for (int index = 0; index < pairs.GetSize(); ++index) {
                    std::cout << "(" << pairs.Get(index).first << ", " << pairs.Get(index).second << ")";
                    if (index + 1 < pairs.GetSize()) std::cout << ", ";
                }
                std::cout << "]\n";
            } else if (operation == 5) {
                ReadOnlyStream<int> input(values);
                OnlineStatistics<int> stats;
                stats.Consume(input);
                std::cout << "count=" << stats.GetCount()
                          << " sum=" << stats.GetSum()
                          << " min=" << stats.GetMin()
                          << " max=" << stats.GetMax()
                          << " mean=" << stats.GetMean()
                          << " median=" << stats.GetMedian()
                          << " window3mean=" << stats.GetWindowMean(3)
                          << "\n";
            }
        }
    }

    return 0;
}
