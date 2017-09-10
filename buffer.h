#ifndef BUFFER_H
#define BUFFER_H

#include <string>
#include <cstring>
#include <vector>

using __Buffer = std::vector<char>;

class Buffer : public __Buffer {
public:
    Buffer(const char *_data, size_t len);
    Buffer(const std::string &str);
    Buffer(size_t sz) : __Buffer(sz) {}
    Buffer() = default;

    Buffer &operator = (const std::string &str);

    template <typename T, typename = typename std::enable_if<std::is_integral<typename std::remove_reference<T>::type>::value>::type>
    Buffer &operator << (const T &obj) {
        for(size_t i = 0; i < sizeof(obj); ++i)
            push_back(reinterpret_cast<const char *>(&obj)[i]);
        return *this;
    }

    Buffer &operator << (const std::string &str);

    Buffer &operator << (const Buffer &buffer);

    void append(void *_src, size_t len);

    template <typename EnumT, typename = typename std::enable_if<std::is_enum<EnumT>::value>::type>
    Buffer &operator << (EnumT type) {
        return this->operator << (static_cast<typename std::underlying_type<EnumT>::type>(type));
    }

    std::string toString() const;

};

#endif // BUFFER_H
