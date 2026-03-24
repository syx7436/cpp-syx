// ============================================================
// Effective Modern C++ Item3: decltype 类型推导
// ============================================================
// 核心结论：
//   decltype 忠实返回表达式或变量的精确类型
//   不会像 auto/模板推导那样剥掉 const 和引用
//   主要用途：函数返回类型需要精确保留引用时，用 decltype(auto)
// ============================================================

#include <iostream>
#include <vector>

// ============================================================
// 【Part 1】decltype vs auto 的核心区别
// auto 会剥掉 const 和引用，decltype 不会
// ============================================================

void part1_decltype_vs_auto(){
    const int i = 0;
    const int& ri = i;

    // auto 推导（Item1 Case3，传值，剥掉 const 和引用）
    auto a = i;    // a = int         （const 被剥）
    auto b = ri;   // b = int         （const 和 & 都被剥）

    // decltype 推导（忠实保留）
    decltype(i)  c = i;   // c = const int    （完整保留）
    decltype(ri) d = i;   // d = const int&   （完整保留）

    // 用来声明类型相同的变量
    decltype(i)  e = 42;  // e = const int
    decltype(ri) f = i;   // f = const int&，必须绑定到 int 对象
}

// ============================================================
// 【Part 2】decltype 最重要的用途：函数返回类型后置
// 当函数返回类型依赖参数类型时使用
// ============================================================

// C++11 写法：返回类型后置，-> decltype(...)
template<typename Container, typename Index>
auto authAndAccess_cpp11(Container& c, Index i)
-> decltype(c[i])
{
    return c[i];
}

// C++14 写法：decltype(auto) 直接推导返回类型
template<typename Container, typename Index>
decltype(auto) authAndAccess_cpp14(Container& c, Index i)
{
    return c[i];
}

// 【对比】直接用 auto 推导返回类型会丢失引用
template<typename Container, typename Index>
auto authAndAccess_auto(Container& c, Index i)
{
    return c[i];   // c[i] 是 int&，但 auto 剥掉引用，实际返回 int
}

void part2_return_type(){
    std::vector<int> v = {1, 2, 3};

    // decltype(auto) 保留引用，可以修改容器内元素
    authAndAccess_cpp14(v, 0) = 99;   // OK，返回 int&，可以赋值
    authAndAccess_cpp11(v, 0) = 88;   // OK，返回 int&，可以赋值

    // auto 丢失引用，返回的是右值，不能赋值
    // authAndAccess_auto(v, 0) = 77;  // 编译报错！返回 int，不是 int&
}

// ============================================================
// 【Part 3】decltype 的推导规则
// 规则1：decltype(变量名)    → 变量声明时的精确类型
// 规则2：decltype(表达式)    → 左值表达式返回T&，右值表达式返回T
// ============================================================

void part3_deduction_rules(){
    int x = 0;
    const int cx = 0;
    const int& rx = cx;

    // 规则1：变量名，返回声明时的精确类型
    decltype(x)  a = 0;   // int
    decltype(cx) b = 0;   // const int
    decltype(rx) c = cx;  // const int&

    // 规则2：表达式
    // 左值表达式 → T&
    // 右值表达式 → T
    int arr[3] = {1, 2, 3};
    decltype(arr[0]) d = arr[0];  // arr[0] 是左值表达式 → int&
}

// ============================================================
// 【Part 4】最容易踩的坑：decltype 加括号
// 变量名加括号后变成表达式，推导结果从 T 变成 T&
// ============================================================

void part4_parentheses_trap(){
    int x = 0;
    decltype(x)   // → int         （变量名，声明类型）
    decltype((x)) // → int&        （加括号变左值表达式，返回引用）
}

// 安全写法
decltype(auto) safe_return()
{
    int x = 42;
    return x;    // OK，返回 int（值拷贝）
}

// 危险写法（不要这样写！）
// decltype(auto) dangerous_return()
// {
//     int x = 42;
//     return (x);  // 返回 int&，但 x 是局部变量，函数结束后悬空引用！
// }
 
// ============================================================
// 【Part 5】decltype(auto) 的实际应用场景
// ============================================================

// 场景：封装容器访问，支持外部修改元素（零拷贝）
template<typename Container, typename Index>
decltype(auto) getElement(Container& c, Index i)
{
    return c[i];  // 返回引用，外部可直接修改容器内元素
}

void part5_practical_usage(){
    std::vector<float> points = {1.0f, 2.0f, 3.0f};

    // 通过引用直接修改，没有拷贝开销
    getElement(points, 0) = 9.9f;   // points[0] 变成 9.9
    // 对比：如果用 auto 返回，修改不生效
    // auto getElement_auto(...) { return c[i]; }  // 返回拷贝
    // getElement_auto(points, 0) = 9.9f;  // 编译报错，右值不能赋值
}

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. decltype vs auto
//    auto  推导：剥掉 const 和引用（Item1 规则）
//    decltype 推导：完整保留，忠实反映类型
//
// 2. decltype 两条规则
//    decltype(变量名)   → 变量声明时的精确类型
//    decltype(表达式)   → 左值表达式→T&，右值表达式→T
//
// 3. decltype(auto) 用于函数返回类型
//    需要返回引用时用 decltype(auto)，不能直接用 auto
//    auto 会剥掉引用，导致返回值拷贝，外部无法通过返回值修改容器
//
// 4. 括号陷阱
//    return x   → decltype(auto) 推导为 T
//    return (x) → decltype(auto) 推导为 T&（危险！局部变量悬空引用）
//    结论：decltype(auto) 做返回类型时，return 里不要随手加括号
//
// ============================================================
 
int main()
{
    part1_decltype_vs_auto();
    part2_return_type();
    part3_deduction_rules();
    part5_practical_usage();
    return 0;
}