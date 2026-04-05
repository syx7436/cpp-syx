#include <iostream>
#include <thread>

void hello(){
    std::cout << "Hello, World!" << std::endl;
    std::cout << "This is a simple C++ program using C++17." << std::endl;
}

int main() {
    std::thread t(hello);
    t.join();
    return 0;
}