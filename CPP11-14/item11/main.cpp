// ============================================================
// Effective Modern C++ Item11: 优先使用 = delete 而非 private
// ============================================================
// 核心结论：
//   老式做法：声明为 private 但不实现来禁用函数
//   现代做法：= delete，意图更清晰，报错更早
//   额外优势：能禁用普通函数和模板特定实例，private 做不到
// ============================================================

#include <iostream>
 
// ============================================================
// 【Part 1】老式做法 vs 现代做法
// ============================================================
 
// 老式做法：private + 不实现（C++11之前）
class OldWidget {
private:
    OldWidget(const OldWidget&);             // 只声明不实现
    OldWidget& operator=(const OldWidget&);  // 友元调用会链接报错，意图不清晰
public:
    OldWidget() {}
};
 
// 现代做法：= delete，放 public，意图清晰，编译期直接报错
class Widget {
public:
    Widget() {}
    Widget(const Widget&)            = delete;  // 明确禁用拷贝构造
    Widget& operator=(const Widget&) = delete;  // 明确禁用拷贝赋值
};
 
void part1_delete_vs_private()
{
    Widget w1;
    // Widget w2(w1);   // 编译报错，意图清晰：禁止拷贝
    // w1 = w1;         // 编译报错
}

// ============================================================
// 【Part 2】= delete 能禁用普通函数，private 做不到
// 防止隐式类型转换导致的意外调用
// ============================================================

bool isLucky(int number) { return number == 7; }
bool isLucky(double)  = delete;  // 禁止 double，防止隐式转换
bool isLucky(bool)    = delete;  // 禁止 bool
bool isLucky(char)    = delete;  // 禁止 char
 
void part2_delete_normal_function()
{
    isLucky(7);       // OK，int
    // isLucky(7.0);  // 编译报错，禁止 double
    // isLucky(true); // 编译报错，禁止 bool
    // isLucky('a');  // 编译报错，禁止 char
}

// ============================================================
// 【Part 3】= delete 能禁用模板特定实例，private 做不到
// ============================================================
 
template<typename T>
void processPointer(T* ptr)
{
    std::cout << "处理指针\n";
    (void)ptr;
}
 
// 禁止特定类型的实例化
template<>
void processPointer<void>(void*) = delete;   // 禁止 void*
template<>
void processPointer<char>(char*) = delete;   // 禁止 char*

void part3_delete_template()
{
    int x = 42;
    processPointer(&x);           // OK，int*
 
    // void* vp = &x;
    // processPointer(vp);        // 编译报错，void* 被禁用
 
    // char* cp = nullptr;
    // processPointer(cp);        // 编译报错，char* 被禁用
}

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. = delete 比 private 的优势
//    意图更清晰，一眼就知道这个函数被禁用了
//    报错更早，编译期直接报错，不是链接期
//    删除函数放 public，错误信息更清晰
//
// 2. = delete 额外能力
//    能禁用普通函数（防止隐式类型转换导致的意外调用）
//    能禁用模板特定实例（private 完全做不到）
//
// 3. 记住一条规则
//    现代 C++ 中禁用函数一律用 = delete
//    永远不用 private + 不实现的老式做法
//
// ==========================================================

int main()
{
    part1_delete_vs_private();
    part2_delete_normal_function();
    part3_delete_template();
    return 0;
}
 