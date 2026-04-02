// ============================================================
// Effective Modern C++ Item6: auto 推导出非预期类型时，使用显式类型初始化
// ============================================================
// 核心结论：
//   auto 有时推导出代理类而不是你想要的真实类型
//   解决方案：auto = static_cast<目标类型>(表达式)
//   既保留 auto 的优点，又明确表达类型转换意图
// ============================================================

#include <iostream>
#include <vector>

// ============================================================
// 【Part 1】vector<bool> 的特殊性
// vector<bool> 底层用位压缩存储，[] 返回的不是 bool&
// 而是一个代理对象 vector<bool>::reference
// ============================================================

void part1_vector_bool_problem()
{
    std::vector<bool> flags = {true, false, true, false};
 
    // 手写类型：安全，编译器把代理对象隐式转换为 bool
    bool f1 = flags[0];   // f1 = true，OK
 
    // auto：危险！推导出代理对象，不是 bool
    auto f2 = flags[0];
    // f2 的类型是 std::vector<bool>::reference
    // 它内部持有一个指向 vector 内部位数据的指针
 
    // 为什么危险：代理对象持有指针，vector 变化后指针可能悬空
    // flags.clear();   // 如果 vector 被清空
    // bool val = f2;   // 未定义行为！f2 内部指针已悬空
}

// ============================================================
// 【Part 2】解决方案：显式类型初始化惯用法
// auto + static_cast，既保留 auto 优点又明确类型意图
// ============================================================

void part2_explicit_type_init()
{
    std::vector<bool> flags = {true, false, true, false};
 
    // 推荐写法：auto + static_cast 显式转换为目标类型
    auto f1 = static_cast<bool>(flags[0]);   // f1 = bool，安全
    auto f2 = static_cast<bool>(flags[1]);   // f2 = bool，安全
 
    // 为什么不直接手写 bool 而用 auto + cast？
    // 1. static_cast 明确告诉读代码的人：这里故意做了类型转换
    // 2. 保留了 auto 强制初始化的优点
    // 3. 如果以后表达式类型变了，cast 会提醒你重新检查
 
    std::cout << "f1: " << f1 << std::endl;  // 1
    std::cout << "f2: " << f2 << std::endl;  // 0
}

// ============================================================
// 【Part 3】代理类的本质
// 代理类是库的实现细节，不应该暴露给用户
// auto 直接拿到代理类就打破了这层封装
// ============================================================

void part3_proxy_class()
{
    std::vector<bool> flags = {true, false, true};
 
    // vector<bool>::reference 是代理类
    // 它重载了 operator bool()，所以能隐式转换为 bool
    // 手写 bool 接收时，隐式转换自动发生，你感知不到代理类的存在
    bool f1 = flags[0];   // 隐式调用 operator bool()，安全
 
    // auto 拿到的是代理类本身，不触发隐式转换
    // 代理类内部持有指针，生命周期和 vector 绑定
    auto f2 = flags[0];   // 拿到 vector<bool>::reference，有隐患
 
    // 正确做法：手动触发转换，拿到真实的 bool 值
    auto f3 = static_cast<bool>(flags[0]);  // 安全
}

// ============================================================
// 【Part 4】同类问题：其他库的代理类
// 不只是 vector<bool>，其他库也有类似的代理对象
// ============================================================

void part4_other_proxy_cases()
{
    // Eigen 矩阵库（自动驾驶常用）也有类似问题
    // Eigen::MatrixXd mat(3, 3);
    // auto val = mat(0, 0);                         // 可能是代理对象
    // auto val = static_cast<double>(mat(0, 0));    // 安全，拿到 double
 
    // 判断是否有代理类问题的方法：
    // 看 [] 或 () 返回的是不是真实类型的引用
    // vector<int>  的 [] 返回 int&        → auto 安全
    // vector<bool> 的 [] 返回 reference   → auto 有隐患，需要 cast
 
    std::vector<int> v = {1, 2, 3};
    auto val = v[0];   // val = int，安全，[] 返回的是 int&
}

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. vector<bool> 是特例
//    底层位压缩存储，[] 返回代理对象 vector<bool>::reference
//    不是真正的 bool&
//
// 2. auto 推导出代理类的隐患
//    代理对象内部持有指针，绑定到容器内部数据
//    容器变化后指针悬空，再用代理对象是未定义行为
//
// 3. 解决方案：显式类型初始化惯用法
//    auto f = static_cast<bool>(flags[0]);
//    既保留 auto 强制初始化的优点
//    又明确表达"我知道这里有类型转换"的意图
//
// 4. 判断什么时候需要 cast
//    [] 或 () 返回真实类型引用 → auto 安全（如 vector<int>）
//    [] 或 () 返回代理对象     → auto + static_cast（如 vector<bool>）
//
// 5. Item5 和 Item6 是一对
//    Item5：多用 auto，避免类型不匹配的 bug
//    Item6：auto 推导出代理类时，用 static_cast 显式转换
//
// ============================================================
 
int main()
{
    part1_vector_bool_problem();
    part2_explicit_type_init();
    part3_proxy_class();
    part4_other_proxy_cases();
    return 0;
}