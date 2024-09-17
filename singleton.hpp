#ifndef __SINGLETON_HPP
#define __SINGLETON_HPP

namespace ljh::utility {

template <class T>
class Singleton {
public:
    static T& Instance(void) {
        static T instance{};

        return instance;
    }

private:
    Singleton<T>() = default;

    Singleton<T>(const Singleton<T>&) = delete;
    Singleton<T>& operator=(const Singleton<T>&) = delete;
};

}  // namespace ljh::utility

#endif