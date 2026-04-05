// ============================================================
// Effective Modern C++ Item8: 优先使用 nullptr 而非 0 或 NULL
// ============================================================
// 核心结论：
//   0 和 NULL 本质是整数，在函数重载和模板推导时会产生歧义
//   nullptr 类型是 std::nullptr_t，能转换为任意指针类型但不是整数
//   现代 C++ 中一律用 nullptr 表示空指针
// ============================================================

#include <iostream>
#include <memory>

// ============================================================
// 【Part 1】0 和 NULL 在函数重载时产生歧义
// ============================================================

void f(int)   { std::cout << "f(int)\n"; }
void f(bool)  { std::cout << "f(bool)\n"; }
void f(void*) { std::cout << "f(void*)\n"; }
 
void part1_overload_ambiguity()
{
    f(0);        // 调用 f(int)，不是 f(void*)，传的是指针却调到整数版本
    //f(NULL);     // 通常是 0 或 0L，调用 f(int)，有些编译器直接报错
    f(nullptr);  // 明确调用 f(void*)，没有任何歧义
}

// ============================================================
// 【Part 2】nullptr 的本质
// 类型是 std::nullptr_t，能转换为任意指针类型，但不是整数
// ============================================================

void part2_nullptr_type()
{
    // nullptr 能隐式转换为任意指针类型
    int*    p1 = nullptr;   // OK
    double* p2 = nullptr;   // OK
    void*   p3 = nullptr;   // OK
 
    // nullptr 不是整数，不能赋给整数类型
    // int x = nullptr;     // 编译报错！
 
    // std::nullptr_t 是 nullptr 的真实类型
    std::nullptr_t np = nullptr;
}

// ============================================================
// 【Part 3】模板推导里 0 和 NULL 的问题
// 模板无法从 0 推导出指针类型
// ============================================================

template<typename FuncType, typename PtrType>
void callWithPtr(FuncType func, PtrType ptr)
{
    func(ptr);
}
 
void processPointer(int* ptr)
{
    if (ptr) std::cout << "有效指针\n";
    else     std::cout << "空指针\n";
}


void part3_template_deduction()
{
    // 0 和 NULL 在模板里推导为整数类型，传给指针参数会报错
    // callWithPtr(processPointer, 0);      // 报错！0 推导为 int，不是 int*
    // callWithPtr(processPointer, NULL);   // 报错！NULL 推导为整数
 
    // nullptr 推导为 std::nullptr_t，能转换为 int*，OK
    callWithPtr(processPointer, nullptr);   // 正确
}

// ============================================================
// 【Part 4】实际代码中的使用
// ============================================================
 
void part4_practical_usage()
{
    // 智能指针判空
    std::shared_ptr<int> sp = nullptr;
    if (sp == nullptr) {
        std::cout << "sp 是空指针\n";
    }
 
    // 原始指针判空
    int* p = nullptr;
    if (p == nullptr) {
        std::cout << "p 是空指针\n";
    }
 
    // 条件判断（nullptr 转换为 bool 是 false）
    if (!p) {
        std::cout << "p 为空\n";
    }
}

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. 0 和 NULL 的问题
//    本质是整数（0 是 int，NULL 通常是 0 或 0L）
//    函数重载时会匹配到 int 版本而不是指针版本
//    模板推导时推导为整数类型，传给指针参数报错
//
// 2. nullptr 的优势
//    类型是 std::nullptr_t，不是整数
//    能隐式转换为任意指针类型
//    函数重载和模板推导时没有歧义
//
// 3. 记住一条规则
//    现代 C++ 中，表示空指针一律用 nullptr
//    永远不用 0 或 NULL 表示指针
//
// ============================================================

int main()
{
    part1_overload_ambiguity();
    part2_nullptr_type();
    part3_template_deduction();
    part4_practical_usage();
    return 0;
}
 