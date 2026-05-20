#include <iostream>
#include <string>
#include <vector>
#include "ArrayQueue.hpp"
#include "ListQueue.hpp"
#include "CircularBufferQueue.hpp"

struct TestStatistics {
    int passed = 0;
    int failed = 0;
    std::vector<std::string> failedCaseNames;
};

static void Report(
    TestStatistics& statistics,
    const std::string& testName,
    const std::string& inputDescription,
    const std::string& expectedValue,
    const std::string& actualValue,
    bool isPassed
) {
    std::cout << (isPassed ? "[PASS] " : "[FAIL] ") << testName << "\n"
              << "  Input:    " << inputDescription << "\n"
              << "  Expected: " << expectedValue << "\n"
              << "  Actual:   " << actualValue << "\n\n";

    if (isPassed) statistics.passed++;
    else {
        statistics.failed++;
        statistics.failedCaseNames.push_back(testName);
    }
}

int main() {
    TestStatistics statistics;

    ArrayQueue<int> arrayQueue;
    arrayQueue.Enqueue(1);
    arrayQueue.Enqueue(2);
    Report(statistics, "ArrayQueue Front", "Enqueue 1,2", "1", std::to_string(arrayQueue.Front()), arrayQueue.Front() == 1);
    Report(statistics, "ArrayQueue Dequeue", "Queue=[1,2]", "1", std::to_string(arrayQueue.Dequeue()), true);

    bool arrayEmptyDequeueException = false;
    try {
        ArrayQueue<int> queue;
        queue.Dequeue();
    } catch (...) {
        arrayEmptyDequeueException = true;
    }
    Report(statistics, "ArrayQueue empty Dequeue", "Dequeue on []", "exception", arrayEmptyDequeueException ? "exception" : "no exception", arrayEmptyDequeueException);

    ListQueue<int> listQueue;
    listQueue.Enqueue(7);
    listQueue.Enqueue(8);
    Report(statistics, "ListQueue Front", "Enqueue 7,8", "7", std::to_string(listQueue.Front()), listQueue.Front() == 7);
    Report(statistics, "ListQueue Dequeue", "Queue=[7,8]", "7", std::to_string(listQueue.Dequeue()), true);

    auto listTryFrontEmpty = ListQueue<int>().TryFront();
    Report(statistics, "ListQueue TryFront empty", "[]", "None", listTryFrontEmpty.IsSome() ? "Some" : "None", !listTryFrontEmpty.IsSome());

    CircularBufferQueue<int> circularQueue(3);
    circularQueue.Enqueue(10);
    circularQueue.Enqueue(11);
    Report(statistics, "Circular Front basic", "capacity=3, enqueue 10,11", "10", std::to_string(circularQueue.Front()), circularQueue.Front() == 10);
    Report(statistics, "Circular Dequeue basic", "Queue=[10,11]", "10", std::to_string(circularQueue.Dequeue()), true);

    // Проверка флага full
    CircularBufferQueue<int> fullCheckQueue(2);
    fullCheckQueue.Enqueue(1);
    fullCheckQueue.Enqueue(2);
    Report(statistics, "Circular IsFull true", "capacity=2, enqueue 1,2", "true", fullCheckQueue.IsFull() ? "true" : "false", fullCheckQueue.IsFull());

    // Проверка wrap-around (кольцевой переход)
    CircularBufferQueue<int> wrapQueue(3);
    wrapQueue.Enqueue(1);
    wrapQueue.Enqueue(2);
    wrapQueue.Enqueue(3);
    wrapQueue.Dequeue(); // удалили 1
    wrapQueue.Enqueue(4); // теперь логическая очередь должна быть [2,3,4]

    int expectedWrapArray[3] = {4, 2, 3};

    int actualWrapArray[3] = {
        wrapQueue.Dequeue(), // 2
        wrapQueue.Dequeue(), // 3
        wrapQueue.Dequeue()  // 4
    };

    bool wrapMatchesExpectedArray =
        (actualWrapArray[0] == expectedWrapArray[0] &&
         actualWrapArray[1] == expectedWrapArray[1] &&
         actualWrapArray[2] == expectedWrapArray[2]);

    Report(
        statistics,
        "Circular wrap-around raw-array compare",
        "capacity=3; enqueue 1,2,3; dequeue; enqueue 4; compare with [4,2,3]",
        "[4,2,3]",
        "[" + std::to_string(actualWrapArray[0]) + "," + std::to_string(actualWrapArray[1]) + "," + std::to_string(actualWrapArray[2]) + "]",
        wrapMatchesExpectedArray
    );

    // TryFront/TryDequeue на непустой
    CircularBufferQueue<int> tryQueue(2);
    tryQueue.Enqueue(42);
    auto tryFrontSome = tryQueue.TryFront();
    auto tryDequeueSome = tryQueue.TryDequeue();
    Report(statistics, "Circular TryFront Some", "[42]", "Some(42)", tryFrontSome.IsSome() ? "Some(" + std::to_string(tryFrontSome.GetOr(0)) + ")" : "None", tryFrontSome.IsSome() && tryFrontSome.GetOr(0) == 42);
    Report(statistics, "Circular TryDequeue Some", "[42]", "Some(42)", tryDequeueSome.IsSome() ? "Some(" + std::to_string(tryDequeueSome.GetOr(0)) + ")" : "None", tryDequeueSome.IsSome() && tryDequeueSome.GetOr(0) == 42);

    // TryFront/TryDequeue на пустой
    auto tryFrontNone = tryQueue.TryFront();
    auto tryDequeueNone = tryQueue.TryDequeue();
    Report(statistics, "Circular TryFront None", "[]", "None", tryFrontNone.IsSome() ? "Some" : "None", !tryFrontNone.IsSome());
    Report(statistics, "Circular TryDequeue None", "[]", "None", tryDequeueNone.IsSome() ? "Some" : "None", !tryDequeueNone.IsSome());

    bool emptyFrontException = false;
    try {
        CircularBufferQueue<int> queue(2);
        queue.Front();
    } catch (...) {
        emptyFrontException = true;
    }
    Report(statistics, "Circular Front on empty", "Front on []", "exception", emptyFrontException ? "exception" : "no exception", emptyFrontException);

    bool emptyDequeueException = false;
    try {
        CircularBufferQueue<int> queue(2);
        queue.Dequeue();
    } catch (...) {
        emptyDequeueException = true;
    }
    Report(statistics, "Circular Dequeue on empty", "Dequeue on []", "exception", emptyDequeueException ? "exception" : "no exception", emptyDequeueException);

    bool fullEnqueueException = false;
    try {
        CircularBufferQueue<int> queue(1);
        queue.Enqueue(1);
        queue.Enqueue(2);
    } catch (...) {
        fullEnqueueException = true;
    }
    Report(statistics, "Circular Enqueue on full", "capacity=1, enqueue 2nd", "exception", fullEnqueueException ? "exception" : "no exception", fullEnqueueException);

    bool badCapacityException = false;
    try {
        CircularBufferQueue<int> queue(0);
        (void)queue;
    } catch (...) {
        badCapacityException = true;
    }
    Report(statistics, "Circular bad capacity", "capacity=0", "exception", badCapacityException ? "exception" : "no exception", badCapacityException);

    std::cout << "===== QUEUE TEST SUMMARY =====\n";
    std::cout << "Passed: " << statistics.passed << "\n";
    std::cout << "Failed: " << statistics.failed << "\n";

    if (!statistics.failedCaseNames.empty()) {
        std::cout << "Failed cases:\n";
        for (const auto& failedCaseName : statistics.failedCaseNames) {
            std::cout << " - " << failedCaseName << "\n";
        }
    }

    return statistics.failed == 0 ? 0 : 1;
}
