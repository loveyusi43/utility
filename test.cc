#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "singleton.hpp"

using namespace ljh::utility;

// 示例使用类
class MyClass {
    friend class Singleton<MyClass>;

public:
    void sayHello() {
        std::cout << "Hello from MyClass!     " << name_ << std::endl;
    }

private:
    MyClass() : name_{"A"} {}
    std::string name_;
};

int main() {
    // 获取单例实例并调用方法
    MyClass t = Singleton<MyClass>::Instance();

    std::cout << &t << std::endl;
    t.sayHello();

    MyClass b = t;

    std::cout << &b << std::endl;

    return 0;
}
