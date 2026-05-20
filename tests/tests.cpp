#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Stack.hpp"

static bool IsEven(const int& x){ return x%2==0; }
static int Square(const int& x){ return x*x; }
static int Sum(const int& a, const int& b){ return a+b; }

struct Stats { int passed=0; int failed=0; std::vector<std::string> failedCases; };

template<class T>
static std::string StackToString(const Stack<T>& s){
    std::ostringstream out; out << "[";
    for(int i=0;i<s.GetSize();++i){ out << s.Raw().Get(i); if(i+1<s.GetSize()) out << ", "; }
    out << "]";
    return out.str();
}

static void Report(Stats& st, const std::string& name, const std::string& input, const std::string& expected, const std::string& actual, bool ok){
    std::cout << (ok ? "[PASS] " : "[FAIL] ") << name << "\n"
              << "  Input:    " << input << "\n"
              << "  Expected: " << expected << "\n"
              << "  Actual:   " << actual << "\n\n";
    if(ok) st.passed++; else { st.failed++; st.failedCases.push_back(name); }
}

int main(){
    Stats st;

    // BLOCK 1: base stack ops
    Stack<int> s; s.Push(1); s.Push(2); s.Push(3);
    Report(st, "Push + GetSize", "Push 1,2,3", "size=3", "size="+std::to_string(s.GetSize()), s.GetSize()==3);
    Report(st, "Peek", "Stack=[1,2,3]", "3", std::to_string(s.Peek()), s.Peek()==3);
    int popped = s.Pop();
    Report(st, "Pop", "Stack=[1,2,3]", "pop=3, size=2", "pop="+std::to_string(popped)+", size="+std::to_string(s.GetSize()), popped==3 && s.GetSize()==2);

    // BLOCK 2: try-semantics
    auto tp = s.TryPeek();
    Report(st, "TryPeek non-empty", "Stack=[1,2]", "Some(2)", tp.IsSome()?"Some("+std::to_string(tp.GetOr(0))+")":"None", tp.IsSome() && tp.GetOr(0)==2);
    Stack<int> empty;
    auto tpe = empty.TryPeek();
    Report(st, "TryPeek empty", "Stack=[]", "None", tpe.IsSome()?"Some":"None", !tpe.IsSome());
    auto tpopEmpty = empty.TryPop();
    Report(st, "TryPop empty", "Stack=[]", "None", tpopEmpty.IsSome()?"Some":"None", !tpopEmpty.IsSome());

    // BLOCK 3: map/where/reduce
    auto mapped = s.Map<int>(Square);
    Report(st, "Map square", "[1,2]", "[1,4]", StackToString(mapped), mapped.GetSize()==2 && mapped.Raw().Get(0)==1 && mapped.Raw().Get(1)==4);
    auto filtered = s.Where(IsEven);
    Report(st, "Where even", "[1,2]", "[2]", StackToString(filtered), filtered.GetSize()==1 && filtered.Raw().Get(0)==2);
    int reduced = s.Reduce(Sum, 0);
    Report(st, "Reduce sum", "[1,2], start=0", "3", std::to_string(reduced), reduced==3);

    // BLOCK 4: concat/substack/find
    int arr[2]={9,10}; Stack<int> s2(arr,2);
    auto concat = s.Concat(s2);
    Report(st, "Concat", "[1,2] + [9,10]", "[1,2,9,10]", StackToString(concat), concat.GetSize()==4 && concat.Raw().Get(2)==9 && concat.Raw().Get(3)==10);
    auto sub = concat.GetSubStack(1,3);
    Report(st, "GetSubStack", "[1,2,9,10], 1..3", "[2,9,10]", StackToString(sub), sub.GetSize()==3 && sub.Raw().Get(0)==2 && sub.Raw().Get(2)==10);
    int patArr[2]={2,9}; Stack<int> pattern(patArr,2);
    int pos = concat.FindSubStack(pattern);
    Report(st, "FindSubStack", "find [2,9] in [1,2,9,10]", "1", std::to_string(pos), pos==1);

    // BLOCK 5: template coverage (double/string)
    Stack<double> sd; sd.Push(1.5); sd.Push(2.5);
    Report(st, "Template double", "Push 1.5,2.5", "size=2", "size="+std::to_string(sd.GetSize()), sd.GetSize()==2);
    Stack<std::string> ss; ss.Push("a"); ss.Push("b");
    Report(st, "Template string", "Push a,b then Peek", "b", ss.Peek(), ss.Peek()=="b");

    // BLOCK 6: negative exception tests
    bool exPop=false; try{ Stack<int> x; x.Pop(); }catch(...){ exPop=true; }
    Report(st, "Exception Pop empty", "Pop on []", "exception", exPop?"exception":"no exception", exPop);
    bool exPeek=false; try{ Stack<int> x; x.Peek(); }catch(...){ exPeek=true; }
    Report(st, "Exception Peek empty", "Peek on []", "exception", exPeek?"exception":"no exception", exPeek);
    bool exSub=false; try{ Stack<int> x; x.Push(1); x.GetSubStack(3,1); }catch(...){ exSub=true; }
    Report(st, "Exception bad substack", "GetSubStack(3,1) on [1]", "exception", exSub?"exception":"no exception", exSub);

    std::cout << "===== TEST SUMMARY =====\n";
    std::cout << "Passed: " << st.passed << "\n";
    std::cout << "Failed: " << st.failed << "\n";
    if(!st.failedCases.empty()){
        std::cout << "Failed cases:\n";
        for(const auto& c : st.failedCases) std::cout << " - " << c << "\n";
    }

    return st.failed==0 ? 0 : 1;
}
