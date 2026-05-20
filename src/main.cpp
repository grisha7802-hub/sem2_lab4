#include <iostream>
#include "Stack.hpp"

static bool IsEvenNumber(const int& value) { return value % 2 == 0; }
static int SquareNumber(const int& value) { return value * value; }
static int SumTwoNumbers(const int& leftValue, const int& rightValue) { return leftValue + rightValue; }

static void PrintIntegerStack(const char* label, const Stack<int>& stackToPrint) {
    std::cout << label << " [";
    for (int elementIndex = 0; elementIndex < stackToPrint.GetSize(); ++elementIndex) {
        std::cout << stackToPrint.Raw().Get(elementIndex);
        if (elementIndex + 1 < stackToPrint.GetSize()) {
            std::cout << ", ";
        }
    }
    std::cout << "]\n";
}

int main() {
    Stack<int> workingStack;

    while (true) {
        std::cout
            << "\n1 Push\n"
            << "2 TryPop\n"
            << "3 TryPeek\n"
            << "4 Show\n"
            << "5 Map (^2)\n"
            << "6 Where (even)\n"
            << "7 Reduce (sum)\n"
            << "8 Concat [9,10]\n"
            << "9 SubStack(start,end)\n"
            << "10 FindSubStack(pattern[2])\n"
            << "0 Exit\n";

        int selectedCommand = 0;
        std::cin >> selectedCommand;
        if (selectedCommand == 0) {
            break;
        }

        if (selectedCommand == 1) {
            int valueToPush = 0;
            std::cin >> valueToPush;
            workingStack.Push(valueToPush);
        }
        else if (selectedCommand == 2) {
            auto popResult = workingStack.TryPop();
            std::cout << (popResult.IsSome() ? "TryPop succeeded" : "TryPop returned None") << "\n";
        }
        else if (selectedCommand == 3) {
            auto peekResult = workingStack.TryPeek();
            std::cout << (peekResult.IsSome() ? "TryPeek succeeded" : "TryPeek returned None") << "\n";
        }
        else if (selectedCommand == 4) {
            PrintIntegerStack("Stack", workingStack);
        }
        else if (selectedCommand == 5) {
            auto mappedStack = workingStack.Map<int>(SquareNumber);
            PrintIntegerStack("Mapped", mappedStack);
        }
        else if (selectedCommand == 6) {
            auto filteredStack = workingStack.Where(IsEvenNumber);
            PrintIntegerStack("Filtered", filteredStack);
        }
        else if (selectedCommand == 7) {
            int reducedValue = workingStack.Reduce(SumTwoNumbers, 0);
            std::cout << "Reduce sum = " << reducedValue << "\n";
        }
        else if (selectedCommand == 8) {
            int appendedValues[2] = {9, 10};
            Stack<int> suffixStack(appendedValues, 2);
            workingStack = workingStack.Concat(suffixStack);
        }
        else if (selectedCommand == 9) {
            int startIndex = 0;
            int endIndex = 0;
            std::cin >> startIndex >> endIndex;
            try {
                auto extractedSubstack = workingStack.GetSubStack(startIndex, endIndex);
                PrintIntegerStack("SubStack", extractedSubstack);
            } catch (const std::exception& exceptionObject) {
                std::cout << exceptionObject.what() << "\n";
            }
        }
        else if (selectedCommand == 10) {
            int firstPatternElement = 0;
            int secondPatternElement = 0;
            std::cin >> firstPatternElement >> secondPatternElement;
            int patternValues[2] = {firstPatternElement, secondPatternElement};
            Stack<int> patternStack(patternValues, 2);
            std::cout << "Found position = " << workingStack.FindSubStack(patternStack) << "\n";
        }
    }

    return 0;
}
