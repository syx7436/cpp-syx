// ============================================================
// Effective Modern C++ Item9: 优先使用 using 而非 typedef
// ============================================================
// 核心结论：
//   using 和 typedef 功能一样，但 using 可读性更好
//   且支持模板别名，typedef 不行
//   现代 C++ 一律用 using 替代 typedef
// ============================================================

#include <vector>
#include <memory>
#include <string>

// ============================================================
// 【Part 1】基本用法对比，两者等价
// ============================================================

// 老写法
typedef std::vector<int>              IntVec_td;
typedef std::unique_ptr<std::string>  StrPtr_td;
 
// 新写法，更直观，左边是别名，右边是类型
using IntVec  = std::vector<int>;
using StrPtr  = std::unique_ptr<std::string>;

// ============================================================
// 【Part 2】using 独有优势：支持模板别名
// ============================================================

// typedef 做不到，要套一层 struct
template<typename T>
struct MyList_td {
    typedef std::vector<T> type;
};
// 使用时很麻烦
// MyList_td<int>::type list;
 
// using 直接支持模板别名
template<typename T>
using MyList = std::vector<T>;
// 使用时很简洁
// MyList<int> list;

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. using 和 typedef 功能等价，但 using 可读性更好
//    using Alias = Type  比  typedef Type Alias 更直观
//
// 2. using 支持模板别名，typedef 不支持
//    需要类型别名带模板参数时，只能用 using
//
// 3. 现代 C++ 一律用 using，不用 typedef
//
// ============================================================
 
int main() { return 0; }
 