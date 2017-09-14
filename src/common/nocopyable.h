/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */


#ifndef CCRAFT_COMMON_NOCOPY_CLASS_H
#define CCRAFT_COMMON_NOCOPY_CLASS_H

namespace ccraft {
namespace common {
class Noncopyable {
protected:
    Noncopyable() {}
    ~Noncopyable() {}
private:  // emphasize the following members are private
    Noncopyable(const Noncopyable&);
    const Noncopyable& operator=(const Noncopyable&);
    Noncopyable(const Noncopyable&&);
    const Noncopyable& operator=(const Noncopyable&&);
};
}
}

#endif //CCRAFT_COMMON_NOCOPY_CLASS_H
