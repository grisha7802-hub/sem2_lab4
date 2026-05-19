#include <iostream>
#include "Stack.hpp"

static bool IsEven(const int& x){ return x%2==0; }
static int Square(const int& x){ return x*x; }
static int Sum(const int& a, const int& b){ return a+b; }

static void PrintStack(const char* name, const Stack<int>& s){
    std::cout << name << " [";
    for(int i=0;i<s.GetSize();++i){ std::cout<<s.Raw().Get(i); if(i+1<s.GetSize()) std::cout<<", "; }
    std::cout << "]\n";
}

int main(){
    Stack<int> s;
    while(true){
        std::cout << "\n1 Push 2 TryPop 3 TryPeek 4 Show 5 Map^2 6 Where even 7 Reduce 8 Concat [9,10] 9 Substack(a,b) 10 FindSub(a,b) 0 Exit\n";
        int c; std::cin>>c; if(c==0) break;
        if(c==1){int v; std::cin>>v; s.Push(v);} 
        else if(c==2){ auto r=s.TryPop(); std::cout<<(r.IsSome()?"TryPop ok":"TryPop none")<<"\n"; }
        else if(c==3){ auto r=s.TryPeek(); std::cout<<(r.IsSome()?"TryPeek ok":"TryPeek none")<<"\n"; }
        else if(c==4){ PrintStack("Stack",s);} 
        else if(c==5){ auto m=s.Map<int>(Square); PrintStack("Mapped",m);} 
        else if(c==6){ auto w=s.Where(IsEven); PrintStack("Filtered",w);} 
        else if(c==7){ std::cout<<"Reduce sum="<<s.Reduce(Sum,0)<<"\n";}
        else if(c==8){ int arr[2]={9,10}; Stack<int> t(arr,2); s=s.Concat(t);} 
        else if(c==9){ int a,b; std::cin>>a>>b; try{ auto sub=s.GetSubStack(a,b); PrintStack("Sub",sub);}catch(const std::exception&e){std::cout<<e.what()<<"\n";} }
        else if(c==10){ int a,b; std::cin>>a>>b; int p[2]={a,b}; Stack<int> pt(p,2); std::cout<<"Find="<<s.FindSubStack(pt)<<"\n"; }
    }
    return 0;
}
