#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Stack.hpp"

static bool IsEvenNumber(const int& value){ return value%2==0; }
static int SquareNumber(const int& value){ return value*value; }
static int SumTwoNumbers(const int& leftValue, const int& rightValue){ return leftValue+rightValue; }

struct TestStatistics { int passed=0; int failed=0; std::vector<std::string> failedCaseNames; };

template<class T>
static std::string StackToString(const Stack<T>& stack){
    std::ostringstream output;
    output << "[";
    for(int elementIndex=0; elementIndex<stack.GetSize(); ++elementIndex){
        output << stack.Raw().Get(elementIndex);
        if(elementIndex + 1 < stack.GetSize()) output << ", ";
    }
    output << "]";
    return output.str();
}

static void Report(TestStatistics& statistics,
                   const std::string& testName,
                   const std::string& inputDescription,
                   const std::string& expectedValue,
                   const std::string& actualValue,
                   bool isPassed){
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

int main(){
    TestStatistics statistics;

    Stack<int> integerStack;
    integerStack.Push(1);
    integerStack.Push(2);
    integerStack.Push(3);

    Report(statistics, "Push + GetSize", "Push 1,2,3", "size=3", "size="+std::to_string(integerStack.GetSize()), integerStack.GetSize()==3);
    Report(statistics, "Peek", "Stack=[1,2,3]", "3", std::to_string(integerStack.Peek()), integerStack.Peek()==3);

    int poppedValue = integerStack.Pop();
    Report(statistics, "Pop", "Pop from [1,2,3]", "pop=3, size=2", "pop="+std::to_string(poppedValue)+", size="+std::to_string(integerStack.GetSize()), poppedValue==3 && integerStack.GetSize()==2);

    auto tryPeekResult = integerStack.TryPeek();
    Report(statistics, "TryPeek non-empty", "Stack=[1,2]", "Some(2)", tryPeekResult.IsSome()?"Some("+std::to_string(tryPeekResult.GetOr(0))+")":"None", tryPeekResult.IsSome() && tryPeekResult.GetOr(0)==2);

    Stack<int> emptyStack;
    auto tryPeekEmptyResult = emptyStack.TryPeek();
    Report(statistics, "TryPeek empty", "Stack=[]", "None", tryPeekEmptyResult.IsSome()?"Some":"None", !tryPeekEmptyResult.IsSome());

    auto tryPopEmptyResult = emptyStack.TryPop();
    Report(statistics, "TryPop empty", "Stack=[]", "None", tryPopEmptyResult.IsSome()?"Some":"None", !tryPopEmptyResult.IsSome());

    auto mappedStack = integerStack.Map<int>(SquareNumber);
    Report(statistics, "Map square", "[1,2]", "[1, 4]", StackToString(mappedStack), mappedStack.GetSize()==2 && mappedStack.Raw().Get(0)==1 && mappedStack.Raw().Get(1)==4);

    auto filteredStack = integerStack.Where(IsEvenNumber);
    Report(statistics, "Where even", "[1,2]", "[2]", StackToString(filteredStack), filteredStack.GetSize()==1 && filteredStack.Raw().Get(0)==2);

    int reducedSum = integerStack.Reduce(SumTwoNumbers, 0);
    Report(statistics, "Reduce sum", "[1,2], start=0", "3", std::to_string(reducedSum), reducedSum==3);

    int suffixValues[2] = {9,10};
    Stack<int> suffixStack(suffixValues,2);
    auto concatenatedStack = integerStack.Concat(suffixStack);
    Report(statistics, "Concat", "[1,2] + [9,10]", "[1, 2, 9, 10]", StackToString(concatenatedStack), concatenatedStack.GetSize()==4 && concatenatedStack.Raw().Get(2)==9 && concatenatedStack.Raw().Get(3)==10);

    auto extractedSubstack = concatenatedStack.GetSubStack(1,3);
    Report(statistics, "GetSubStack", "[1,2,9,10], 1..3", "[2, 9, 10]", StackToString(extractedSubstack), extractedSubstack.GetSize()==3 && extractedSubstack.Raw().Get(0)==2 && extractedSubstack.Raw().Get(2)==10);

    int patternValues[2] = {2,9};
    Stack<int> patternStack(patternValues,2);
    int foundPosition = concatenatedStack.FindSubStack(patternStack);
    Report(statistics, "FindSubStack", "Find [2,9] in [1,2,9,10]", "1", std::to_string(foundPosition), foundPosition==1);

    Stack<double> doubleStack;
    doubleStack.Push(1.5);
    doubleStack.Push(2.5);
    Report(statistics, "Template double", "Push 1.5,2.5", "size=2", "size="+std::to_string(doubleStack.GetSize()), doubleStack.GetSize()==2);

    Stack<std::string> stringStack;
    stringStack.Push("a");
    stringStack.Push("b");
    Report(statistics, "Template string", "Push a,b then Peek", "b", stringStack.Peek(), stringStack.Peek()=="b");

    bool popExceptionThrown=false;
    try { Stack<int> localEmpty; localEmpty.Pop(); } catch (...) { popExceptionThrown=true; }
    Report(statistics, "Exception Pop empty", "Pop on []", "exception", popExceptionThrown?"exception":"no exception", popExceptionThrown);

    bool peekExceptionThrown=false;
    try { Stack<int> localEmpty; localEmpty.Peek(); } catch (...) { peekExceptionThrown=true; }
    Report(statistics, "Exception Peek empty", "Peek on []", "exception", peekExceptionThrown?"exception":"no exception", peekExceptionThrown);

    bool subStackExceptionThrown=false;
    try { Stack<int> localStack; localStack.Push(1); localStack.GetSubStack(3,1); } catch (...) { subStackExceptionThrown=true; }
    Report(statistics, "Exception bad substack", "GetSubStack(3,1) on [1]", "exception", subStackExceptionThrown?"exception":"no exception", subStackExceptionThrown);

    std::cout << "===== TEST SUMMARY =====\n";
    std::cout << "Passed: " << statistics.passed << "\n";
    std::cout << "Failed: " << statistics.failed << "\n";
    if(!statistics.failedCaseNames.empty()){
        std::cout << "Failed cases:\n";
        for(const auto& failedCase : statistics.failedCaseNames) std::cout << " - " << failedCase << "\n";
    }

    return statistics.failed==0 ? 0 : 1;
}
