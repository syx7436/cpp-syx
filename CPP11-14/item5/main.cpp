// ============================================================
// Effective Modern C++ Item5: 优先使用 auto 而非显式类型声明
// ============================================================
// 核心结论：
//   auto 不只是少打几个字，它能避免未初始化、类型不匹配、
//   隐式拷贝等真实 bug 和性能问题
// ============================================================

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

// ============================================================
// 【Part 1】auto 强制初始化
// 手写类型可以不初始化，auto 不行，编译期堵死隐患
// ============================================================

void part1_forced_initialization()
{
    int x;      // 未初始化，值不确定，潜在 bug
    // auto y;  // 编译报错！auto 必须有初始值才能推导
 
    auto a = 0;          // OK
    auto b = 0.0f;       // OK
    auto c = false;      // OK
}

// ============================================================
// 【Part 2】避免有符号和无符号比较
// size() 返回 size_t（无符号），用 int 接收会有隐患
// ============================================================

void part2_size_t_mismatch()
{
    std::vector<int> v = {1, 2, 3, 4, 5};
 
    // 错误写法：int 和 size_t 比较，有符号和无符号混用
    // 编译器警告，极端情况下结果出错
    for (int i = 0; i < (int)v.size(); i++) { }
 
    // 正确写法：auto 推导出 size_t，类型完全匹配
    for (auto i = 0u; i < v.size(); i++) { }
 
    // 更好的写法：直接用范围 for
    for (const auto& elem : v) { (void)elem; }
 
    // size 也用 auto 接
    auto sz = v.size();   // sz = std::size_t，不是 int
}

// ============================================================
// 【Part 3】遍历 unordered_map 的隐式拷贝陷阱
// 这是最隐蔽的性能问题，编译不报错但每次循环都在拷贝
// ============================================================

void part3_map_iteration()
{
    std::unordered_map<std::string, int> m = {
        {"lidar", 64},
        {"camera", 8},
        {"radar", 4}
    };
 
    // 错误写法：类型写错了！
    // unordered_map 的 value_type 是 pair<const string, int>
    // key 是 const string，不是 string
    // 类型不匹配，编译器对每个元素做隐式转换，产生临时拷贝！
    for (const std::pair<std::string, int>& p : m) {
        // 看起来是引用，实际上绑定的是编译器创建的临时拷贝
        // 每次循环都在偷偷拷贝，性能损耗且不易察觉
        (void)p;
    }
 
    // 正确写法：auto 推导出精确类型，直接绑定引用，零拷贝
    for (const auto& p : m) {
        // p 的类型是 const pair<const string, int>&
        // 完全匹配，没有任何拷贝
        (void)p;
    }
}

// ============================================================
// 【Part 4】lambda 类型只有编译器知道，必须用 auto
// ============================================================

void part4_lambda_type()
{
    // lambda 的真实类型是编译器生成的匿名类，无法手写
    auto compare = [](const int& a, const int& b) {
        return a < b;
    };
 
    std::vector<int> v = {3, 1, 4, 1, 5};
    std::sort(v.begin(), v.end(), compare);
 
    // 如果不用 auto，只能用 std::function，但有性能开销
    // std::function<bool(const int&, const int&)> compare2 = ...
    // std::function 有虚函数调用开销，auto 直接持有闭包类型没有这个开销
}

// ============================================================
// 【Part 5】auto 让代码更容易维护
// 类型改变时不需要到处修改
// ============================================================

void part5_maintainability()
{
    std::vector<int> v = {1, 2, 3};
 
    // 手写类型：如果 vector 改成其他容器，这里要跟着改
    std::vector<int>::iterator it1 = v.begin();
 
    // auto：容器类型变了这里不用动
    auto it2 = v.begin();
}

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. auto 强制初始化
//    手写类型可以不初始化留下隐患，auto 编译期直接报错
//
// 2. 避免 size_t 和 int 混用
//    v.size() 返回 size_t，用 int 接有符号无符号比较隐患
//    用 auto 接收，类型完全匹配
//
// 3. unordered_map 遍历必须用 auto
//    value_type 是 pair<const string, int>，不是 pair<string, int>
//    手写类型写错了编译不报错，但每次循环都在隐式拷贝
//    用 const auto& 直接绑定，零拷贝
//
// 4. lambda 必须用 auto
//    lambda 类型是编译器生成的匿名类，手写不了
//    用 std::function 接收有虚函数调用开销，auto 没有
//
// 5. 易维护
//    类型改变时 auto 自动适应，手写类型要到处改
//
// ============================================================
 
int main()
{
    part1_forced_initialization();
    part2_size_t_mismatch();
    part3_map_iteration();
    part4_lambda_type();
    part5_maintainability();
    return 0;
}