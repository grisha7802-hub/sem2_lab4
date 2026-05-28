#include <iostream>
#include <string>
#include <vector>

#include "LazySequence.hpp"
#include "Stream.hpp"

static int addInt(int a, int b) { return a + b; }
static int squareInt(int x) { return x * x; }
static bool isEvenInt(int x) { return x % 2 == 0; }

static void Assert(bool condition, const std::string& name) {
    if (!condition) throw std::runtime_error("FAILED: " + name);
}

int main() {
    try {
        std::vector<int> fibSeed = {0, 1};
        auto fib = Stream<int>::Recurrence(
            fibSeed,
            [](const std::vector<int>& prefix) {
                int n = static_cast<int>(prefix.size());
                return prefix[n - 1] + prefix[n - 2];
            },
            Cardinal::Infinity()
        );

        std::vector<int> fibFirst10 = fib.Take(10);
        std::vector<int> expectedFib = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
        Assert(fibFirst10 == expectedFib, "Fibonacci first 10");

        auto fibSquares = fib.Map<int>(squareInt);
        std::vector<int> squares5 = fibSquares.Take(5);
        std::vector<int> expectedSquares = {0, 1, 1, 4, 9};
        Assert(squares5 == expectedSquares, "Map over infinite stream");

        int finiteDataRaw[6] = {1, 2, 3, 4, 5, 6};
        auto finite = Stream<int>::FromArray(finiteDataRaw, 6);
        auto evens = finite.Where(isEvenInt);
        std::vector<int> evenValues = evens.Take(3);
        std::vector<int> expectedEvens = {2, 4, 6};
        Assert(evenValues == expectedEvens, "Where on finite stream");

        int reduced = finite.Reduce<int>(addInt, 0);
        Assert(reduced == 21, "Reduce finite stream");

        int zippedRaw[4] = {10, 20, 30, 40};
        auto second = Stream<int>::FromArray(zippedRaw, 4);
        auto zipped = finite.Zip(second);
        auto zippedValues = zipped.Take(4);
        Assert(zippedValues[0].first == 1 && zippedValues[0].second == 10, "Zip first pair");
        Assert(zippedValues[3].first == 4 && zippedValues[3].second == 40, "Zip fourth pair");

        LazySequence<int> lazyFinite(finiteDataRaw, 6);
        Assert(lazyFinite.Get(0) == 1, "LazySequence Get first");
        Assert(lazyFinite.GetLast() == 6, "LazySequence GetLast");

        std::cout << "LR4 tests: ALL PASSED\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cout << ex.what() << "\n";
        return 1;
    }
}
