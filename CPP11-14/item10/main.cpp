// ============================================================
// Effective Modern C++ Item10: 优先使用 enum class 而非 enum
// ============================================================
// 核心结论：
//   老式 enum 有两个问题：枚举名泄漏到外部作用域、隐式转换为整数
//   enum class 解决了这两个问题，现代 C++ 一律用 enum class
//   唯一例外：需要隐式转换为整数时才用老式 enum
// ============================================================

#include <iostream>
#include <tuple>
#include <string>

// ============================================================
// 【Part 1】老式 enum 的两个问题
// ============================================================

void part1_old_enum_problems()
{
    // 问题1：枚举名泄漏到外部作用域，容易冲突
    enum Color { black, white, red };
    // auto white = false;  // 报错！white 被 enum 占了
 
    // 问题2：隐式转换为整数，毫无意义的比较编译器不报错
    Color c = red;
    if (c < 14.5) {   // Color 和 double 比较，编译通过但毫无意义
        // 危险！
    }
}

// ============================================================
// 【Part 2】enum class 解决两个问题
// ============================================================

void part2_enum_class()
{
    // 枚举名不泄漏，不会和外部冲突
    auto white = false;          // OK，不冲突
    //Color c = Color::red;        // 必须加作用域，清晰明确
 
    // 不能隐式转换为整数，编译器报错
    // if (c < 14.5) { }         // 编译报错！
    // int x = c;                // 编译报错！
 
    // 需要整数时显式转换
    //int x = static_cast<int>(c); // OK，明确表达转换意图
}

// ============================================================
// 【Part 3】enum class 支持前置声明，减少头文件依赖
// ============================================================

// 老式 enum 某些情况不能前置声明
// enum Color2;   // 可能报错
 
// enum class 直接支持前置声明
enum class Direction;          // OK
enum class Direction : int;    // OK，还可以指定底层类型
 
enum class Direction { up, down, left, right };  // 定义

// ============================================================
// 【Part 4】老式 enum 唯一有用的场景
// 需要隐式转换为整数时（如用枚举值当下标）
// ============================================================

void part4_old_enum_usage()
{
    // 用枚举值当 tuple 下标，老式 enum 更方便
    using PersonInfo = std::tuple<std::string, int, double>;
    enum Field { name, age, score };  // 老式 enum，隐式转换为 0, 1, 2
 
    PersonInfo p = {"张三", 25, 95.5};
    auto personName  = std::get<name>(p);   // 隐式转换为 0，方便
    auto personAge   = std::get<age>(p);    // 隐式转换为 1
    auto personScore = std::get<score>(p);  // 隐式转换为 2
 
    // enum class 就要写 static_cast，麻烦很多
    // std::get<static_cast<int>(Field::name)>(p);
}

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. 老式 enum 的两个问题
//    枚举名泄漏到外部作用域，容易和变量名冲突
//    隐式转换为整数，无意义的比较编译器不报错
//
// 2. enum class 的优势
//    枚举名不泄漏，必须加作用域 Color::red
//    不能隐式转换为整数，需要显式 static_cast
//    支持前置声明，减少头文件依赖
//
// 3. 唯一用老式 enum 的场景
//    需要隐式转换为整数时，比如用枚举值当 tuple 下标
//
// 4. 记住一条规则
//    现代 C++ 默认用 enum class，有隐式转整数需求才用老式 enum
//
// ============================================================
 
int main()
{
    part1_old_enum_problems();
    part2_enum_class();
    part4_old_enum_usage();
    return 0;
}

 
