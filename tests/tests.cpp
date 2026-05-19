#include <iostream>
#include <string>
#include "Stack.hpp"

static bool IsEven(const int& x){ return x%2==0; }
static int Square(const int& x){ return x*x; }
static int Sum(const int& a, const int& b){ return a+b; }

int main(){
    int failed=0, passed=0;
    auto chk=[&](const char* n, bool c){ std::cout<<(c?"[PASS] ":"[FAIL] ")<<n<<"\n"; if(c) passed++; else failed++; };

    Stack<int> s; s.Push(1); s.Push(2); s.Push(3);
    chk("int peek", s.Peek()==3);
    chk("int trypeek", s.TryPeek().IsSome());
    chk("int trypop", s.TryPop().IsSome() && s.GetSize()==2);
    chk("int map", s.Map<int>(Square).Raw().Get(1)==4);
    chk("int where", s.Where(IsEven).GetSize()==1);
    chk("int reduce", s.Reduce(Sum,0)==3);

    Stack<std::string> ss; ss.Push("a"); ss.Push("b");
    chk("string peek", ss.Peek()=="b");
    chk("string trypop", ss.TryPop().GetOr("x")=="b");

    Stack<double> sd; sd.Push(1.5); sd.Push(2.5);
    chk("double size", sd.GetSize()==2);

    int a[2]={9,10}; Stack<int> s2(a,2); auto c=s.Concat(s2);
    chk("concat", c.GetSize()==4 && c.Raw().Get(2)==9);
    int b[2]={2,9}; Stack<int> patt(b,2); chk("find sub", c.FindSubStack(patt)==1);
    chk("substack", c.GetSubStack(1,3).GetSize()==3);

    bool ex1=false; try{ Stack<int> e; e.Pop(); }catch(...){ ex1=true; }
    chk("exception pop empty", ex1);
    bool ex2=false; try{ Stack<int> e; e.Peek(); }catch(...){ ex2=true; }
    chk("exception peek empty", ex2);
    bool ex3=false; try{ Stack<int> e; e.Push(1); e.GetSubStack(2,1);}catch(...){ ex3=true; }
    chk("exception bad substack", ex3);

    std::cout<<"Passed="<<passed<<" Failed="<<failed<<"\n";
    return failed==0?0:1;
}
