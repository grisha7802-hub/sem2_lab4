#include <iostream>
#include <vector>

#include "Stream.hpp"

static int squareInt(int x) { return x * x; }
static bool isEvenInt(int x) { return x % 2 == 0; }

static void PrintVector(const std::vector<int>& values, const char* title) {
    std::cout << title << ": [";
    for (std::size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i];
        if (i + 1 < values.size()) std::cout << ", ";
    }
    std::cout << "]\n";
}

int RunLr4Console() {
    std::vector<int> fibSeed = {0, 1};
    auto fib = Stream<int>::Recurrence(
        fibSeed,
        [](const std::vector<int>& prefix) {
            int n = static_cast<int>(prefix.size());
            return prefix[n - 1] + prefix[n - 2];
        },
        Cardinal::Infinity()
    );

    int finiteRaw[8] = {3, 1, 4, 1, 5, 9, 2, 6};
    auto finite = Stream<int>::FromArray(finiteRaw, 8);

    while (true) {
        std::cout << "\nLR4 menu\n"
                  << "1 Show first N Fibonacci\n"
                  << "2 Show squares of first N Fibonacci\n"
                  << "3 Show even numbers from finite stream\n"
                  << "0 Exit\n";
        int cmd = 0;
        std::cin >> cmd;
        if (cmd == 0) break;

        if (cmd == 1) {
            int n = 0;
            std::cin >> n;
            PrintVector(fib.Take(n), "Fibonacci");
        } else if (cmd == 2) {
            int n = 0;
            std::cin >> n;
            auto sq = fib.Map<int>(squareInt);
            PrintVector(sq.Take(n), "Fib squares");
        } else if (cmd == 3) {
            auto evens = finite.Where(isEvenInt);
            PrintVector(evens.Take(3), "Evens");
        }
    }

    return 0;
}
