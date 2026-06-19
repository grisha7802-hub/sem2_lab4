#include <iostream>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "ArraySequence.hpp"
#include "LazySequence.hpp"
#include "OnlineStatistics.hpp"
#include "Stream.hpp"

static int addInt(int a, int b) { return a + b; }
static int squareInt(int x) { return x * x; }
static int doubleInt(int x) { return x * 2; }
static bool isEvenInt(int x) { return x % 2 == 0; }
static bool isGreaterThanTenInt(int x) { return x > 10; }
static int parseInt(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (...) {
        throw std::invalid_argument("parse int");
    }
}
static std::string serializeInt(const int& value) { return std::to_string(value); }

struct TestStatistics {
    int passed = 0;
    int failed = 0;
    ArraySequence<std::string> failedCaseNames;
};

template<class T>
static std::string SequenceToString(const Sequence<T>& values) {
    std::ostringstream output;
    output << "[";
    for (int index = 0; index < values.GetSize(); ++index) {
        output << values.Get(index);
        if (index + 1 < values.GetSize()) output << ", ";
    }
    output << "]";
    return output.str();
}

template<class First, class Second>
static std::string PairSequenceToString(const Sequence<std::pair<First, Second>>& values) {
    std::ostringstream output;
    output << "[";
    for (int index = 0; index < values.GetSize(); ++index) {
        output << "(" << values.Get(index).first << ", " << values.Get(index).second << ")";
        if (index + 1 < values.GetSize()) output << ", ";
    }
    output << "]";
    return output.str();
}

template<class T>
static bool SequenceEqualsArray(const Sequence<T>& actual, const T* expected, int expectedCount) {
    if (actual.GetSize() != expectedCount) return false;
    for (int index = 0; index < expectedCount; ++index) {
        if (!(actual.Get(index) == expected[index])) return false;
    }
    return true;
}

template<class First, class Second>
static bool PairSequenceEqualsArray(const Sequence<std::pair<First, Second>>& actual,
                                    const std::pair<First, Second>* expected,
                                    int expectedCount) {
    if (actual.GetSize() != expectedCount) return false;
    for (int index = 0; index < expectedCount; ++index) {
        if (!(actual.Get(index) == expected[index])) return false;
    }
    return true;
}

static void Report(TestStatistics& statistics,
                   const std::string& testName,
                   const std::string& inputDescription,
                   const std::string& expectedValue,
                   const std::string& actualValue,
                   bool isPassed) {
    std::cout << (isPassed ? "[PASS] " : "[FAIL] ") << testName << "\n"
              << "  Input:    " << inputDescription << "\n"
              << "  Expected: " << expectedValue << "\n"
              << "  Actual:   " << actualValue << "\n\n";

    if (isPassed) statistics.passed++;
    else {
        statistics.failed++;
        statistics.failedCaseNames.Append(testName);
    }
}

template<class Func>
static void ReportThrows(TestStatistics& statistics,
                         const std::string& testName,
                         const std::string& inputDescription,
                         const std::string& expectedException,
                         Func action) {
    std::string actualValue = "no exception";
    bool isPassed = false;

    try {
        action();
    } catch (const std::exception& exception) {
        actualValue = exception.what();
        isPassed = actualValue == expectedException;
    } catch (...) {
        actualValue = "unknown exception";
    }

    Report(statistics, testName, inputDescription, expectedException, actualValue, isPassed);
}

int main() {
    TestStatistics statistics;

    int finiteDataRaw[6] = {1, 2, 3, 4, 5, 6};
    auto finite = Stream<int>::FromArray(finiteDataRaw, 6);

    auto firstSix = finite.Take(6);
    int expectedFirstSix[6] = {1, 2, 3, 4, 5, 6};
    Report(statistics,
           "Take finite stream",
           "[1, 2, 3, 4, 5, 6], count=6",
           "[1, 2, 3, 4, 5, 6]",
           SequenceToString(firstSix),
           SequenceEqualsArray(firstSix, expectedFirstSix, 6));

    auto takeZero = finite.Take(0);
    Report(statistics,
           "Take zero from finite stream",
           "[1, 2, 3, 4, 5, 6], count=0",
           "[]",
           SequenceToString(takeZero),
           takeZero.GetSize() == 0);

    int thirdValue = finite.Get(2);
    Report(statistics,
           "Get by index from finite stream",
           "[1, 2, 3, 4, 5, 6], index=2",
           "3",
           std::to_string(thirdValue),
           thirdValue == 3);

    auto squares = finite.Map<int>(squareInt);
    auto squareValues = squares.Take(6);
    int expectedSquares[6] = {1, 4, 9, 16, 25, 36};
    Report(statistics,
           "Map square over finite stream",
           "[1, 2, 3, 4, 5, 6], square",
           "[1, 4, 9, 16, 25, 36]",
           SequenceToString(squareValues),
           SequenceEqualsArray(squareValues, expectedSquares, 6));

    auto doubled = finite.Map<int>(doubleInt);
    auto doubledValues = doubled.Take(6);
    int expectedDoubled[6] = {2, 4, 6, 8, 10, 12};
    Report(statistics,
           "Map double over finite stream",
           "[1, 2, 3, 4, 5, 6], double",
           "[2, 4, 6, 8, 10, 12]",
           SequenceToString(doubledValues),
           SequenceEqualsArray(doubledValues, expectedDoubled, 6));

    auto evens = finite.Where(isEvenInt);
    auto evenValues = evens.Take(3);
    int expectedEvens[3] = {2, 4, 6};
    Report(statistics,
           "Where even on finite stream",
           "[1, 2, 3, 4, 5, 6], even",
           "[2, 4, 6]",
           SequenceToString(evenValues),
           SequenceEqualsArray(evenValues, expectedEvens, 3));

    auto greaterThanTen = finite.Where(isGreaterThanTenInt);
    ReportThrows(statistics,
                 "Where empty result rejects Get",
                 "[1, 2, 3, 4, 5, 6], value > 10, Get(0)",
                 "IndexOutOfRange",
                 [&greaterThanTen]() { greaterThanTen.Get(0); });

    int reduced = finite.Reduce<int>(addInt, 0);
    Report(statistics,
           "Reduce finite stream",
           "[1, 2, 3, 4, 5, 6], start=0",
           "21",
           std::to_string(reduced),
           reduced == 21);

    int reducedWithInitial = finite.Reduce<int>(addInt, 10);
    Report(statistics,
           "Reduce finite stream with initial",
           "[1, 2, 3, 4, 5, 6], start=10",
           "31",
           std::to_string(reducedWithInitial),
           reducedWithInitial == 31);

    int zippedRaw[4] = {10, 20, 30, 40};
    auto second = Stream<int>::FromArray(zippedRaw, 4);
    auto zipped = finite.Zip(second);
    auto zippedValues = zipped.Take(4);
    std::pair<int, int> expectedZipped[4] = {
        {1, 10},
        {2, 20},
        {3, 30},
        {4, 40}
    };
    Report(statistics,
           "Zip streams",
           "[1, 2, 3, 4, 5, 6] zip [10, 20, 30, 40]",
           "[(1, 10), (2, 20), (3, 30), (4, 40)]",
           PairSequenceToString(zippedValues),
           PairSequenceEqualsArray(zippedValues, expectedZipped, 4));

    int shorterRaw[2] = {100, 200};
    auto shorter = Stream<int>::FromArray(shorterRaw, 2);
    auto zippedShorter = finite.Zip(shorter);
    auto zippedShorterValues = zippedShorter.Take(2);
    std::pair<int, int> expectedZippedShorter[2] = {
        {1, 100},
        {2, 200}
    };
    Report(statistics,
           "Zip truncates to shorter stream",
           "[1, 2, 3, 4, 5, 6] zip [100, 200]",
           "[(1, 100), (2, 200)]",
           PairSequenceToString(zippedShorterValues),
           PairSequenceEqualsArray(zippedShorterValues, expectedZippedShorter, 2));

    ReportThrows(statistics,
                 "Zip shorter rejects out of range",
                 "[1, 2, 3, 4, 5, 6] zip [100, 200], Get(2)",
                 "IndexOutOfRange",
                 [&zippedShorter]() { zippedShorter.Get(2); });

    LazySequence<int> lazyFinite(finiteDataRaw, 6);
    int firstValue = lazyFinite.GetFirst();
    Report(statistics,
           "LazySequence GetFirst",
           "[1, 2, 3, 4, 5, 6]",
           "1",
           std::to_string(firstValue),
           firstValue == 1);

    int lastValue = lazyFinite.GetLast();
    Report(statistics,
           "LazySequence GetLast",
           "[1, 2, 3, 4, 5, 6]",
           "6",
           std::to_string(lastValue),
           lastValue == 6);

    LazySequence<int>* subsequence = lazyFinite.GetSubsequence(1, 3);
    int expectedSubsequence[3] = {2, 3, 4};
    ArraySequence<int> subsequenceValues;
    subsequenceValues.Append(subsequence->Get(0));
    subsequenceValues.Append(subsequence->Get(1));
    subsequenceValues.Append(subsequence->Get(2));
    Report(statistics,
           "LazySequence GetSubsequence",
           "[1, 2, 3, 4, 5, 6], range=1..3",
           "[2, 3, 4]",
           SequenceToString(subsequenceValues),
           SequenceEqualsArray(subsequenceValues, expectedSubsequence, 3));
    delete subsequence;

    LazySequence<int> mutableLazy(finiteDataRaw, 6);
    mutableLazy.Prepend(0)->Append(7)->InsertAt(99, 3);
    int expectedMutableValues[9] = {0, 1, 2, 99, 3, 4, 5, 6, 7};
    ArraySequence<int> mutableValues;
    for (int index = 0; index < 9; ++index) {
        mutableValues.Append(mutableLazy.Get(index));
    }
    Report(statistics,
           "LazySequence mutation operations",
           "Prepend 0, append 7, insert 99 at index 3",
           "[0, 1, 2, 99, 3, 4, 5, 6, 7]",
           SequenceToString(mutableValues),
           SequenceEqualsArray(mutableValues, expectedMutableValues, 9));

    ReportThrows(statistics,
                 "Take rejects negative count",
                 "[1, 2, 3, 4, 5, 6], count=-1",
                 "count < 0",
                 [&finite]() { finite.Take(-1); });

    ReportThrows(statistics,
                 "LazySequence Get rejects negative index",
                 "[1, 2, 3, 4, 5, 6], index=-1",
                 "IndexOutOfRange",
                 [&lazyFinite]() { lazyFinite.Get(-1); });

    LazySequence<int> emptyLazy;
    ReportThrows(statistics,
                 "LazySequence GetLast rejects empty sequence",
                 "[]",
                 "IndexOutOfRange",
                 [&emptyLazy]() { emptyLazy.GetLast(); });

    ReportThrows(statistics,
                 "LazySequence InsertAt rejects bad index",
                 "[1, 2, 3, 4, 5, 6], InsertAt(10, 99)",
                 "IndexOutOfRange",
                 [&lazyFinite]() { lazyFinite.InsertAt(99, 10); });

    int firstRaw[3] = {1, 2, 3};
    int secondRawValues[2] = {4, 5};
    LazySequence<int> firstLazy(firstRaw, 3);
    LazySequence<int> secondLazy(secondRawValues, 2);
    LazySequence<int>* concatenated = firstLazy.Concat(&secondLazy);
    int expectedConcat[5] = {1, 2, 3, 4, 5};
    ArraySequence<int> concatValues;
    for (int index = 0; index < 5; ++index) concatValues.Append(concatenated->Get(index));
    Report(statistics,
           "LazySequence Concat",
           "[1,2,3] + [4,5]",
           "[1, 2, 3, 4, 5]",
           SequenceToString(concatValues),
           SequenceEqualsArray(concatValues, expectedConcat, 5));
    delete concatenated;

    int naturalsSeedRaw[1] = {0};
    ArraySequence<int> naturalsSeed(naturalsSeedRaw, 1);
    LazySequence<int> naturals(
        naturalsSeed,
        [](const Sequence<int>& prefix) {
            return prefix.GetSize();
        },
        Cardinal::Infinity()
    );

    LazySequence<int>* infiniteEvens = naturals.Where(isEvenInt);
    int expectedInfiniteEvens[6] = {0, 2, 4, 6, 8, 10};
    ArraySequence<int> infiniteEvenValues;
    for (int index = 0; index < 6; ++index) infiniteEvenValues.Append(infiniteEvens->Get(index));
    Report(statistics,
           "LazySequence Where over infinite sequence",
           "naturals where even, first 6",
           "[0, 2, 4, 6, 8, 10]",
           SequenceToString(infiniteEvenValues),
           SequenceEqualsArray(infiniteEvenValues, expectedInfiniteEvens, 6));
    delete infiniteEvens;

    bool appendInfiniteDidNotThrow = true;
    try {
        naturals.Append(999);
    } catch (...) {
        appendInfiniteDidNotThrow = false;
    }
    Report(statistics,
           "LazySequence Append to infinite is deferred",
           "naturals.Append(999)",
           "no exception",
           appendInfiniteDidNotThrow ? "no exception" : "exception",
           appendInfiniteDidNotThrow);

    Generator<int> generator(
        [](std::size_t index) { return index < 3; },
        [](std::size_t index) { return static_cast<int>(index * 10); }
    );
    bool generatorOk = generator.HasNext()
        && generator.GetNext() == 0
        && generator.GetNext() == 10
        && generator.GetNext() == 20
        && !generator.HasNext()
        && !generator.TryGetNext().IsSome();
    Report(statistics,
           "Generator sequential access",
           "0, 10, 20",
           "0, 10, 20, end",
           generatorOk ? "0, 10, 20, end" : "bad generator state",
           generatorOk);

    ReadOnlyStream<int> textStream(std::string("7 8 9"), parseInt);
    bool textStreamOk = textStream.Read() == 7
        && textStream.GetPosition() == 1
        && textStream.Seek(0) == 0
        && textStream.Read() == 7;
    Report(statistics,
           "ReadOnlyStream string read and seek",
           "\"7 8 9\"",
           "first=7, seek=0, first=7",
           textStreamOk ? "first=7, seek=0, first=7" : "bad stream state",
           textStreamOk);

    ArraySequence<int> streamTarget;
    WriteOnlyStream<int> sequenceWriter(&streamTarget);
    sequenceWriter.Write(11);
    sequenceWriter.Write(12);
    int expectedWrittenSequence[2] = {11, 12};
    Report(statistics,
           "WriteOnlyStream writes to Sequence",
           "write 11, 12",
           "[11, 12]",
           SequenceToString(streamTarget),
           SequenceEqualsArray(streamTarget, expectedWrittenSequence, 2));

    const char* streamFileName = "lr4_stream_test.tmp";
    {
        WriteOnlyStream<int> fileWriter(streamFileName, serializeInt);
        fileWriter.Open();
        fileWriter.Write(21);
        fileWriter.Write(22);
        fileWriter.Close();
    }
    ReadOnlyStream<int> fileReader(streamFileName, parseInt);
    fileReader.Open();
    bool fileStreamOk = fileReader.Read() == 21
        && fileReader.Read() == 22
        && fileReader.IsEndOfStream();
    fileReader.Close();
    std::remove(streamFileName);
    Report(statistics,
           "ReadOnlyStream and WriteOnlyStream file roundtrip",
           "21, 22",
           "21, 22, end",
           fileStreamOk ? "21, 22, end" : "bad file stream state",
           fileStreamOk);

    ReadOnlyStream<int> statsStream(std::string("4 1 7 3"), parseInt);
    OnlineStatistics<int> statisticsTask;
    statisticsTask.Consume(statsStream);
    bool statisticsOk = statisticsTask.GetCount() == 4
        && statisticsTask.GetSum() == 15
        && statisticsTask.GetMin() == 1
        && statisticsTask.GetMax() == 7
        && statisticsTask.GetMean() == 3.75
        && statisticsTask.GetMedian() == 3.5
        && statisticsTask.GetWindowMin(2) == 3
        && statisticsTask.GetWindowMax(2) == 7
        && statisticsTask.GetWindowMean(2) == 5.0;
    Report(statistics,
           "Online statistics thematic task",
           "4 1 7 3",
           "count=4 sum=15 min=1 max=7 mean=3.75 median=3.5 windowMean=5",
           statisticsOk ? "count=4 sum=15 min=1 max=7 mean=3.75 median=3.5 windowMean=5" : "bad statistics",
           statisticsOk);

    OnlineStatistics<int> emptyStatistics;
    ReportThrows(statistics,
                 "OnlineStatistics rejects empty median",
                 "empty stats, GetMedian",
                 "Statistics is empty",
                 [&emptyStatistics]() { emptyStatistics.GetMedian(); });

    ArraySequence<int> appendItems;
    appendItems.Append(8);
    appendItems.Append(9);
    LazySequence<int> appendLazy(finiteDataRaw, 6);
    appendLazy.Append(&appendItems)->RemoveAt(1);
    int expectedAppendRemove[7] = {1, 3, 4, 5, 6, 8, 9};
    ArraySequence<int> appendRemoveValues;
    for (int index = 0; index < 7; ++index) appendRemoveValues.Append(appendLazy.Get(index));
    Report(statistics,
           "LazySequence append Sequence and RemoveAt",
           "[1..6] append [8,9], remove index 1",
           "[1, 3, 4, 5, 6, 8, 9]",
           SequenceToString(appendRemoveValues),
           SequenceEqualsArray(appendRemoveValues, expectedAppendRemove, 7));

    auto lazyGenerator = lazyFinite.CreateGenerator();
    bool lazyGeneratorOk = lazyGenerator.GetNext() == 1
        && lazyGenerator.GetNext() == 2
        && lazyGenerator.GetNext() == 3;
    Report(statistics,
           "LazySequence owned generator",
           "[1,2,3,...]",
           "1, 2, 3",
           lazyGeneratorOk ? "1, 2, 3" : "bad generator",
           lazyGeneratorOk);

    ReadWriteStream<int> readWriteStream;
    readWriteStream.Write(31);
    readWriteStream.Write(32);
    readWriteStream.Seek(0);
    bool readWriteOk = readWriteStream.Read() == 31
        && readWriteStream.Read() == 32
        && readWriteStream.IsEndOfStream();
    Report(statistics,
           "ReadWriteStream memory roundtrip",
           "write 31, 32",
           "31, 32, end",
           readWriteOk ? "31, 32, end" : "bad read-write stream",
           readWriteOk);

    ReportThrows(statistics,
                 "ReadOnlyStream rejects end read",
                 "\"7\", Read twice",
                 "EndOfStream",
                 []() {
                     ReadOnlyStream<int> stream(std::string("7"), parseInt);
                     stream.Read();
                     stream.Read();
                 });

    ReportThrows(statistics,
                 "ReadOnlyStream rejects bad seek",
                 "\"7\", Seek(2)",
                 "IndexOutOfRange",
                 []() {
                     ReadOnlyStream<int> stream(std::string("7"), parseInt);
                     stream.Seek(2);
                 });

    ReportThrows(statistics,
                 "ReadOnlyStream rejects closed read",
                 "\"7\", Close(), Read()",
                 "Stream is closed",
                 []() {
                     ReadOnlyStream<int> stream(std::string("7"), parseInt);
                     stream.Close();
                     stream.Read();
                 });

    ReportThrows(statistics,
                 "ReadOnlyStream rejects malformed input",
                 "\"1 bad 2\"",
                 "parse int",
                 []() {
                     ReadOnlyStream<int> stream(std::string("1 bad 2"), parseInt);
                 });

    const char* largeFileName = "lr4_large_stream_test.tmp";
    {
        std::ofstream largeFile(largeFileName);
        for (int index = 0; index < 50000; ++index) {
            largeFile << index << '\n';
        }
    }
    ReadOnlyStream<int> largeFileReader(largeFileName, parseInt);
    largeFileReader.Open();
    long long largeFileSum = 0;
    while (!largeFileReader.IsEndOfStream()) {
        largeFileSum += largeFileReader.Read();
    }
    largeFileReader.Close();
    std::remove(largeFileName);
    bool largeFileOk = largeFileSum == 1249975000LL;
    Report(statistics,
           "Large file stream boundary test",
           "50000 generated integers",
           "sum=1249975000",
           largeFileOk ? "sum=1249975000" : std::to_string(largeFileSum),
           largeFileOk);

    auto naturalStream = Stream<int>::Recurrence(
        naturalsSeed,
        [](const Sequence<int>& prefix) {
            return prefix.GetSize();
        },
        Cardinal::Infinity()
    );
    auto firstHundredThousand = naturalStream.Take(100000);
    bool stressOk = firstHundredThousand.GetSize() == 100000
        && firstHundredThousand.Get(0) == 0
        && firstHundredThousand.Get(99999) == 99999;
    Report(statistics,
           "Large lazy stream boundary test",
           "first 100000 naturals",
           "size=100000 first=0 last=99999",
           stressOk ? "size=100000 first=0 last=99999" : "bad large stream",
           stressOk);

    std::cout << "===== TEST SUMMARY =====\n";
    std::cout << "Passed: " << statistics.passed << "\n";
    std::cout << "Failed: " << statistics.failed << "\n";
    if (statistics.failedCaseNames.GetSize() > 0) {
        std::cout << "Failed cases:\n";
        for (int index = 0; index < statistics.failedCaseNames.GetSize(); ++index) {
            std::cout << " - " << statistics.failedCaseNames.Get(index) << "\n";
        }
    }

    return statistics.failed == 0 ? 0 : 1;
}
