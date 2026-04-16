// ============================================================
// Effective Modern C++ Item22: Pimpl 惯用法
// ============================================================
// 核心结论：
//   Pimpl = Pointer to Implementation
//   把类的实现细节藏到 cpp 里，头文件只留前置声明和 unique_ptr
//   好处：减少头文件依赖，改实现只重新编译一个 cpp
//   注意：析构函数必须在 cpp 里定义，不能在头文件里
// ============================================================

// ============================================================
// 【文件结构说明】
// 实际项目中 Pimpl 分两个文件：
//   Lidar.h   → 头文件，对外暴露接口，不包含实现细节
//   Lidar.cpp → 实现文件，包含所有细节和第三方依赖
// 这里为了演示放在一个文件里
// ============================================================

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ============================================================
// 【Part 1】没有 Pimpl 的情况（问题演示）
// ============================================================

// 没有 Pimpl 时，头文件暴露所有依赖
// #include "LidarSDK.h"    ← 只要这个改了
// #include "PointCloud.h"  ← 或者这个改了
// 所有包含这个头文件的文件全部重新编译！

class LidarWithoutPimpl {
public:
    void connect() { std::cout << "连接激光雷达: " << ip_ << "\n"; }
    void scan()    { std::cout << "扫描点云\n"; }
private:
    // 所有实现细节直接暴露在头文件里
    std::string ip_;              // 依赖 <string>
    std::vector<float> buffer_;   // 依赖 <vector>
    // LidarSDK::Handle handle_;  // 依赖第三方SDK头文件
    // PointCloud cloud_;         // 依赖点云头文件
    // 任何一个依赖改了，所有包含这个头文件的文件都要重新编译
};

// ============================================================
// 【Part 2】用 Pimpl 改造（正确做法）
// ============================================================

// ---- 模拟 Lidar.h 的内容 ----
class Lidar {
public:
    Lidar();
    ~Lidar();                    // 只声明，必须在 cpp 里定义

    // 拷贝和移动也需要显式处理
    Lidar(const Lidar& other);
    Lidar& operator=(const Lidar& other);
    Lidar(Lidar&& other);
    Lidar& operator=(Lidar&& other);

    void connect(const std::string& ip);
    void scan();
    void printInfo() const;

private:
    struct Impl;                          // 只声明，不定义
    std::unique_ptr<Impl> pImpl_;         // 指向实现的指针
};

// ---- 模拟 Lidar.cpp 的内容 ----

// Impl 的完整定义在 cpp 里
// 这里才 include 第三方头文件，改了不会影响其他文件
struct Lidar::Impl {
    std::string ip;
    std::vector<float> buffer;
    bool connected = false;
    // LidarSDK::Handle handle;  // 实际项目里放第三方类型
    // PointCloud cloud;

    void connect(const std::string& ip_) {
        ip = ip_;
        connected = true;
        std::cout << "连接激光雷达: " << ip << "\n";
    }

    void scan() {
        if (!connected) {
            std::cout << "未连接，无法扫描\n";
            return;
        }
        buffer = {1.0f, 2.0f, 3.0f};  // 模拟点云数据
        std::cout << "扫描完成，点数: " << buffer.size() << "\n";
    }
};

// 构造函数：创建 Impl
Lidar::Lidar() : pImpl_(std::make_unique<Impl>()) {}

// 析构函数：必须在 cpp 里定义！
// 因为这里 Impl 才是完整类型，unique_ptr 的删除器才知道怎么 delete
// 在头文件里写 = default 会报错，因为那时候 Impl 还不完整
Lidar::~Lidar() = default;

// 拷贝构造：深拷贝 Impl
Lidar::Lidar(const Lidar& other)
    : pImpl_(std::make_unique<Impl>(*other.pImpl_)) {}

// 拷贝赋值
Lidar& Lidar::operator=(const Lidar& other) {
    *pImpl_ = *other.pImpl_;
    return *this;
}

// 移动构造：直接移动 unique_ptr
Lidar::Lidar(Lidar&& other) = default;

// 移动赋值
Lidar& Lidar::operator=(Lidar&& other) = default;

// 成员函数委托给 Impl
void Lidar::connect(const std::string& ip) { pImpl_->connect(ip); }
void Lidar::scan()                          { pImpl_->scan(); }
void Lidar::printInfo() const {
    std::cout << "IP: " << pImpl_->ip
              << ", 已连接: " << pImpl_->connected << "\n";
}

// ============================================================
// 【Part 3】为什么析构函数不能在头文件里写
// ============================================================

// 错误示范（不要这样写）：
// class Lidar {
//     ~Lidar() = default;  // 报错！
//     // 编译器处理这行时，Impl 只是前置声明，是不完整类型
//     // unique_ptr 的删除器不知道 Impl 有多大，怎么析构
// };

// 正确做法：
// .h 里只声明  ~Lidar();
// .cpp 里定义  Lidar::~Lidar() = default;
// 因为 .cpp 里 Impl 已经完整定义了

// ============================================================
// 【Part 4】编译依赖对比
// ============================================================

// 没有 Pimpl：
//   LidarSDK.h 改了
//   → Lidar.h 依赖它，Lidar.h 也变了
//   → 所有 #include "Lidar.h" 的文件全部重新编译（可能几百个）
//
// 用了 Pimpl：
//   LidarSDK.h 改了
//   → 只有 Lidar.cpp 依赖它
//   → 只重新编译 Lidar.cpp，其他文件完全不受影响

// ============================================================
// 【核心总结】
// ============================================================
//
// 1. Pimpl 的目的
//    把实现细节从头文件搬到 cpp
//    头文件只留前置声明和 unique_ptr<Impl>
//    减少编译依赖，改实现不触发大规模重新编译
//
// 2. 析构函数必须在 cpp 里定义
//    头文件里 Impl 是不完整类型，unique_ptr 不知道怎么 delete
//    cpp 里 Impl 完整定义后，= default 才合法
//
// 3. 拷贝和移动需要显式处理
//    移动：= default 即可，直接移动 unique_ptr
//    拷贝：需要深拷贝 Impl，手动实现
//
// 4. 适用场景
//    大型项目中频繁被包含的头文件
//    依赖第三方 SDK 或复杂内部类型的类
//    自动驾驶中传感器、算法模块等需要隐藏实现的组件
//
// ============================================================

int main()
{
    Lidar lidar;
    lidar.connect("192.168.1.10");
    lidar.scan();
    lidar.printInfo();

    // 移动
    Lidar lidar2 = std::move(lidar);
    lidar2.printInfo();

    // 拷贝
    Lidar lidar3 = lidar2;
    lidar3.printInfo();

    return 0;
}