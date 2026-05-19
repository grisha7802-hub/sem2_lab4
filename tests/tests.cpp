#include <iostream>
#include "Stack.hpp"

static bool IsEven(const int& x){ return x%2==0; }
static int Square(const int& x){ return x*x; }
static int Sum(const int& a, const int& b){ return a+b; }

int main(){
    int failed=0, passed=0;
    auto chk=[&](const char* n, bool c){ std::cout<<(c?"[PASS] ":"[FAIL] ")<<n<<"\n"; if(c) passed++; else failed++; };

    Stack<int> s; s.Push(1); s.Push(2); s.Push(3);
    chk("size==3", s.GetSize()==3);
    chk("peek==3", s.Peek()==3);
    chk("pop==3", s.Pop()==3);
    chk("size==2", s.GetSize()==2);

    auto m=s.Map<int>(Square);
    chk("map square", m.Raw().Get(0)==1 && m.Raw().Get(1)==4);

    auto w=s.Where(IsEven);
    chk("where even", w.GetSize()==1 && w.Raw().Get(0)==2);

    chk("reduce sum", s.Reduce(Sum,0)==3);

    int a[2]={9,10}; Stack<int> s2(a,2);
    auto c=s.Concat(s2);
    chk("concat", c.GetSize()==4 && c.Raw().Get(2)==9);

    int b[2]={2,9}; Stack<int> patt(b,2);
    chk("find substack", c.FindSubStack(patt)==1);

    auto sub=c.GetSubStack(1,3);
    chk("substack", sub.GetSize()==3 && sub.Raw().Get(0)==2 && sub.Raw().Get(2)==10);

    std::cout<<"Passed="<<passed<<" Failed="<<failed<<"\n";
    return failed==0?0:1;
}
