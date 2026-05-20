#include <iostream>
#include <string>
#include <vector>
#include "ArrayQueue.hpp"
#include "ListQueue.hpp"
#include "CircularBufferQueue.hpp"

struct TestStatistics { int passed=0; int failed=0; std::vector<std::string> failedCaseNames; };

static void Report(TestStatistics& statistics,const std::string& testName,const std::string& inputDescription,const std::string& expectedValue,const std::string& actualValue,bool isPassed){
    std::cout << (isPassed ? "[PASS] " : "[FAIL] ") << testName << "\n"
              << "  Input:    " << inputDescription << "\n"
              << "  Expected: " << expectedValue << "\n"
              << "  Actual:   " << actualValue << "\n\n";
    if(isPassed) statistics.passed++; else { statistics.failed++; statistics.failedCaseNames.push_back(testName);} }

int main(){
    TestStatistics statistics;

    ArrayQueue<int> arrayQueue; arrayQueue.Enqueue(1); arrayQueue.Enqueue(2);
    Report(statistics,"ArrayQueue Front","Enqueue 1,2","1",std::to_string(arrayQueue.Front()),arrayQueue.Front()==1);
    Report(statistics,"ArrayQueue Dequeue","Queue=[1,2]","1",std::to_string(arrayQueue.Dequeue()),true);

    ListQueue<int> listQueue; listQueue.Enqueue(7); listQueue.Enqueue(8);
    Report(statistics,"ListQueue Front","Enqueue 7,8","7",std::to_string(listQueue.Front()),listQueue.Front()==7);
    Report(statistics,"ListQueue Dequeue","Queue=[7,8]","7",std::to_string(listQueue.Dequeue()),true);

    CircularBufferQueue<int> circularQueue(3); circularQueue.Enqueue(10); circularQueue.Enqueue(11);
    Report(statistics,"Circular Front","Enqueue 10,11","10",std::to_string(circularQueue.Front()),circularQueue.Front()==10);
    Report(statistics,"Circular Dequeue","Queue=[10,11]","10",std::to_string(circularQueue.Dequeue()),true);

    bool exceptionOnEmpty=false; try{ ArrayQueue<int> q; q.Dequeue(); }catch(...){ exceptionOnEmpty=true; }
    Report(statistics,"ArrayQueue empty Dequeue","Dequeue on []","exception",exceptionOnEmpty?"exception":"no exception",exceptionOnEmpty);

    bool exceptionOnFull=false; try{ CircularBufferQueue<int> q(1); q.Enqueue(1); q.Enqueue(2);}catch(...){ exceptionOnFull=true; }
    Report(statistics,"Circular full Enqueue","capacity=1, enqueue 2nd","exception",exceptionOnFull?"exception":"no exception",exceptionOnFull);

    auto tryFrontNone = ListQueue<int>().TryFront();
    Report(statistics,"ListQueue TryFront empty","[]","None",tryFrontNone.IsSome()?"Some":"None",!tryFrontNone.IsSome());

    std::cout << "===== QUEUE TEST SUMMARY =====\nPassed: " << statistics.passed << "\nFailed: " << statistics.failed << "\n";
    if(!statistics.failedCaseNames.empty()){ std::cout << "Failed cases:\n"; for(const auto& failedCaseName:statistics.failedCaseNames) std::cout << " - " << failedCaseName << "\n"; }
    return statistics.failed==0 ? 0 : 1;
}
