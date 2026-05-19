#include <iostream>
#include <chrono>
#include "Stack.hpp"

using clk=std::chrono::high_resolution_clock;

template<typename F>
long long ms(F fn){ auto s=clk::now(); fn(); auto e=clk::now(); return std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count(); }

int main(){
    int sizes[3]={1000,10000,100000};
    std::cout<<"op,size,ms\n";
    for(int n: sizes){
        auto tpush=ms([&]{ Stack<int> s; for(int i=0;i<n;++i) s.Push(i); });
        Stack<int> s; for(int i=0;i<n;++i) s.Push(i);
        auto tpop=ms([&]{ while(!s.IsEmpty()) s.Pop(); });
        Stack<int> a; for(int i=0;i<n;++i) a.Push(i);
        Stack<int> b; for(int i=0;i<n;++i) b.Push(i);
        auto tconcat=ms([&]{ auto c=a.Concat(b); (void)c; });
        Stack<int> pat; pat.Push(n/2); pat.Push(n/2+1);
        auto tfind=ms([&]{ auto p=a.FindSubStack(pat); (void)p; });
        std::cout<<"push,"<<n<<","<<tpush<<"\n";
        std::cout<<"pop,"<<n<<","<<tpop<<"\n";
        std::cout<<"concat,"<<n<<","<<tconcat<<"\n";
        std::cout<<"findSub,"<<n<<","<<tfind<<"\n";
    }
}
