// ============================================================
// Effective Modern C++ Item1: 模板类型推导
// ============================================================
// 核心问题：给定下面的模板，传入 expr 后，T 和 param 是什么类型？
//
// template<typename T>
// void f(ParamType param);
// f(expr);
//
// 分三种 Case 讨论，取决于 ParamType 的写法
// ============================================================

#include <iostream>
#include <typeinfo>

// ============================================================
// 【Case 1】ParamType 是引用或指针（但不是通用引用）
// 规则：
//   1. 如果 expr 有引用，先忽略引用
//   2. 然后用 expr 的类型和 ParamType 模式匹配，推导 T
// ============================================================

template<typename T>
void f_lref(T& param) {}

template<typename T>
void f_const_lref(const T& param) {}

template<typename T>
void f_ptr(T* param) {}

void case1(){
    int x = 27;
    const int cx = x;
    const int& rx = x;
    // --- T& ---
    // x  是 int        → T = int,        param = int&
    // cx 是 const int  → T = const int,  param = const int&  （const 保留进 T）
    // rx 是 const int& → 先忽略&，T = const int, param = const int&
    f_lref(x);
    f_lref(cx);
    f_lref(rx);
    // --- const T& ---
    // x  是 int        → T = int,  param = const int&
    // cx 是 const int  → T = int,  param = const int&  （const 不重复进 T）
    // rx 是 const int& → T = int,  param = const int&
    f_const_lref(x);
    f_const_lref(cx);
    f_const_lref(rx);
    // --- T* ---
    const int* px = &cx;
    // px 是 const int* → T = const int, param = const int*
    f_ptr(px);
}

// ============================================================
// 【Case 2】ParamType 是通用引用 T&&
// 规则：
//   1. expr 是左值 → T 和 ParamType 都推导为左值引用
//      （唯一一种 T 本身被推导为引用的情况）
//   2. expr 是右值 → 按 Case1 规则推导
// ============================================================
template<typename T>
void f_universal(T&& param) {}   // ParamType = T&&，通用引用

void case2(){
    int x = 27;
    const int cx = x;
    const int& rx = cx;
    // 传左值：
    // x  是 int 左值        → T = int&,        param = int&        （引用折叠：int& && = int&）
    // cx 是 const int 左值  → T = const int&,  param = const int&
    // rx 是 const int& 左值 → T = const int&,  param = const int&
    f_universal(x);
    f_universal(cx);
    f_universal(rx);
    // 传右值：
    // 27 是 int 右值         → T = int,  param = int&&
    f_universal(27);
    // 只要出现左值引用，结果就是左值引用
}

// ============================================================
// 【Case 3】ParamType 是普通传值 T
// 规则：
//   1. 忽略 expr 的引用
//   2. 忽略 expr 的 const 和 volatile
//   （因为 param 是独立拷贝，原来的约束和它无关）
// ============================================================
template<typename T>
void f_value(T param) {}   // ParamType = T，纯传值

void case3(){
    int x = 27;
    const int cx = x;
    const int& rx = cx;
    // x  是 int        → T = int
    // cx 是 const int  → T = int   （const 被剥掉，param 是拷贝，可以修改）
    // rx 是 const int& → T = int   （const 和 & 都被剥掉）
    f_value(x);
    f_value(cx);
    f_value(rx);
    // 【特殊情况】const 指针传值
    // ptr 自身的 const 被剥掉（指针本身是拷贝）
    // ptr 指向内容的 const 被保留（那块内存的属性不变）
    const char* const ptr = "Fun with pointers";
    f_value(ptr);
    // T = const char*
    //     ↑保留（内容的const）  ↑丢掉（指针本身的const）
}

// ============================================================
// 【补充】数组和函数的退化规则
// 规则：
//   传值 → 退化为指针，数组大小丢失
//   传引用 → 保留完整类型，数组大小保留
// ============================================================
template<typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept  { return N; }

template<typename T, std::size_t N>
void f_array_value(T param[]) {}     // 等价于 T* param，退化

template<typename T, std::size_t N>
void f_array_ref(T (&data)[N]) {     // 传引用，不退化
    // data 用法和普通数组名完全一样
    // data[i] 访问元素
    // N 是编译期常量，不需要手动传 size
    for (std::size_t i = 0; i < N; i++) {
        (void)data[i];
    }
}

void case4_array(){
    int arr[5] = {1, 2, 3, 4, 5};
    const char name[] = "J. P. Briggs";  // 类型是 const char[13]
 
    // 传值：退化为指针
    // f_array_value(arr) → T = int*，大小丢失
 
    // 传引用：保留完整类型
    // f_array_ref(arr)   → T = int, N = 5，param 类型 = int(&)[5]
    f_array_ref(arr);
 
    // 编译期获取数组大小
    constexpr auto size = arraySize(arr);   // size = 5，编译期确定
    (void)size;
}

// ============================================================
// 【四条核心总结】
// ============================================================
//
// 1. 有引用的实参，推导时引用被忽略
//    const int& rx → 推导时当 const int 看
//
// 2. 通用引用 T&&，左值实参被特殊对待
//    左值 → T 推导为左值引用（唯一 T 会变成引用的情况）
//    右值 → 按 Case1 规则正常推导
//
// 3. 传值时，实参的 const 和 volatile 被忽略
//    const int cx → T = int（param 是拷贝，独立对象）
//    但指针指向内容的 const 保留，指针本身的 const 被剥
//
// 4. 数组和函数传值退化为指针，传引用不退化
//    传值：T = int*（大小丢失）
//    传引用：T = int[N]（大小保留，可编译期使用）
//
// ============================================================
 
int main()
{
    case1();
    case2();
    case3();
    case4_array();
    return 0;
}
 