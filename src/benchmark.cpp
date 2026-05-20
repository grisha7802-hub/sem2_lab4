#include <iostream>
#include <chrono>
#include "Stack.hpp"

using HighResolutionClock = std::chrono::high_resolution_clock;

template<typename Callable>
long long MeasureMilliseconds(Callable measuredOperation) {
    auto startTime = HighResolutionClock::now();
    measuredOperation();
    auto endTime = HighResolutionClock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

int main() {
    int benchmarkSizes[3] = {1000, 10000, 100000};
    std::cout << "operation,size,milliseconds\n";

    for (int currentSize : benchmarkSizes) {
        auto pushTimeMs = MeasureMilliseconds([&] {
            Stack<int> benchmarkStack;
            for (int index = 0; index < currentSize; ++index) {
                benchmarkStack.Push(index);
            }
        });

        Stack<int> stackForPopBenchmark;
        for (int index = 0; index < currentSize; ++index) {
            stackForPopBenchmark.Push(index);
        }
        auto popTimeMs = MeasureMilliseconds([&] {
            while (!stackForPopBenchmark.IsEmpty()) {
                stackForPopBenchmark.Pop();
            }
        });

        Stack<int> leftStack;
        Stack<int> rightStack;
        for (int index = 0; index < currentSize; ++index) {
            leftStack.Push(index);
            rightStack.Push(index);
        }
        auto concatTimeMs = MeasureMilliseconds([&] {
            auto concatenatedStack = leftStack.Concat(rightStack);
            (void)concatenatedStack;
        });

        Stack<int> searchPattern;
        searchPattern.Push(currentSize / 2);
        searchPattern.Push(currentSize / 2 + 1);
        auto findSubstackTimeMs = MeasureMilliseconds([&] {
            int foundPosition = leftStack.FindSubStack(searchPattern);
            (void)foundPosition;
        });

        std::cout << "push," << currentSize << "," << pushTimeMs << "\n";
        std::cout << "pop," << currentSize << "," << popTimeMs << "\n";
        std::cout << "concat," << currentSize << "," << concatTimeMs << "\n";
        std::cout << "findSubStack," << currentSize << "," << findSubstackTimeMs << "\n";
    }

    return 0;
}
