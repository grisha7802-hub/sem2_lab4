#include <iostream>
#include <chrono>
#include "ArrayQueue.hpp"
#include "ListQueue.hpp"
#include "CircularBufferQueue.hpp"

using HighResolutionClock = std::chrono::high_resolution_clock;

template<typename Callable>
long long MeasureMilliseconds(Callable measuredOperation) {
    auto startTime = HighResolutionClock::now();
    measuredOperation();
    auto endTime = HighResolutionClock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

int main(){
    int testSizes[3] = {1000, 10000, 100000};
    std::cout << "queue_impl,operation,size,milliseconds\n";
    for(int size : testSizes){
        auto arrayEnqueue = MeasureMilliseconds([&]{ ArrayQueue<int> queue; for(int i=0;i<size;++i) queue.Enqueue(i); });
        auto arrayDequeue = MeasureMilliseconds([&]{ ArrayQueue<int> queue; for(int i=0;i<size;++i) queue.Enqueue(i); for(int i=0;i<size;++i) queue.Dequeue(); });

        auto listEnqueue = MeasureMilliseconds([&]{ ListQueue<int> queue; for(int i=0;i<size;++i) queue.Enqueue(i); });
        auto listDequeue = MeasureMilliseconds([&]{ ListQueue<int> queue; for(int i=0;i<size;++i) queue.Enqueue(i); for(int i=0;i<size;++i) queue.Dequeue(); });

        auto circularEnqueue = MeasureMilliseconds([&]{ CircularBufferQueue<int> queue(size+1); for(int i=0;i<size;++i) queue.Enqueue(i); });
        auto circularDequeue = MeasureMilliseconds([&]{ CircularBufferQueue<int> queue(size+1); for(int i=0;i<size;++i) queue.Enqueue(i); for(int i=0;i<size;++i) queue.Dequeue(); });

        std::cout << "ArrayQueue,enqueue," << size << "," << arrayEnqueue << "\n";
        std::cout << "ArrayQueue,dequeue," << size << "," << arrayDequeue << "\n";
        std::cout << "ListQueue,enqueue," << size << "," << listEnqueue << "\n";
        std::cout << "ListQueue,dequeue," << size << "," << listDequeue << "\n";
        std::cout << "CircularBufferQueue,enqueue," << size << "," << circularEnqueue << "\n";
        std::cout << "CircularBufferQueue,dequeue," << size << "," << circularDequeue << "\n";
    }
}
