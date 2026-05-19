#include <iostream>
#include "Stack.hpp"

static bool IsEven(const int& x){ return x%2==0; }
static int Square(const int& x){ return x*x; }
static int Sum(const int& a, const int& b){ return a+b; }

static void PrintStack(const char* name, const Stack<int>& s){
    std::cout << name << " [";
    for(int i=0;i<s.GetSize();++i){
        std::cout << s.Raw().Get(i);
        if(i+1<s.GetSize()) std::cout << ", ";
    }
    std::cout << "]\n";
}

int main(){
    Stack<int> s;
    while(true){
        std::cout << "\n1 Push 2 Pop 3 Peek 4 Show 5 Map^2 6 Where even 7 Reduce sum 0 Exit\n";
        int c; std::cin >> c;
        if(c==0) break;
        if(c==1){ int v; std::cin>>v; s.Push(v);} 
        else if(c==2){ try{ std::cout<<"Pop="<<s.Pop()<<"\n";}catch(const std::exception& e){ std::cout<<e.what()<<"\n";}}
        else if(c==3){ try{ std::cout<<"Peek="<<s.Peek()<<"\n";}catch(const std::exception& e){ std::cout<<e.what()<<"\n";}}
        else if(c==4){ PrintStack("Stack",s);} 
        else if(c==5){ auto m=s.Map<int>(Square); PrintStack("Mapped",m);} 
        else if(c==6){ auto w=s.Where(IsEven); PrintStack("Filtered",w);} 
        else if(c==7){ std::cout<<"Reduce sum="<<s.Reduce(Sum,0)<<"\n";}
    }
    return 0;
}
