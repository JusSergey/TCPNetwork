#include "buffer.h"

Buffer::Buffer(const char *_data, size_t len) : __Buffer(len) {
    memcpy(this->data(), _data, len);
}

Buffer::Buffer(const std::string &str) : __Buffer(str.size()+1) {
    auto strsz = str.size();
    memcpy(this->data(), str.data(), strsz);
    data()[strsz] = '\0';
}

Buffer &Buffer::operator = (const std::string &str) {
    this->resize(str.size());
    memcpy(this->data(), str.data(), str.size());
    return *this;
}

void Buffer::append(void *_src, size_t len) {
    reserve(size() + len + 4);
    for (size_t i = 0; i < len; ++i)
        push_back(static_cast<const char*>(_src)[i]);
}

std::string Buffer::toString() const {

    std::string str;
    const size_t len = size();
    if (len < 1)
        return str;

    str.resize(len-1);

    memcpy(&str[0], data(), len);

    return std::move(str);
}

Buffer &Buffer::operator <<(const Buffer &buffer) {
    const size_t sz = buffer.size();
    const char *dt = buffer.data();
    reserve(size() + sz + 4);
    for(size_t i = 0; i < sz; ++i)
        push_back(dt[i]);
    return *this;
}

Buffer &Buffer::operator <<(const std::string &str) {
    const size_t sz = str.size();
    const char *dt = str.data();
    reserve(size() + sz + 4);
    for(size_t i = 0; i < sz; ++i)
        push_back(dt[i]);
    return *this;
}
