#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>
#include <cstring>

#include <functional>
#include <future>
#include <iostream>
#include <vector>
#include <type_traits>

constexpr auto _64KB = 65536;
constexpr auto DELAY_TIME = 3; //ms

namespace Net {

struct SocketFD;
class Buffer;

using __Buffer           = std::vector<char>;
using CallbackRecv       = std::function<void(Buffer, SocketFD)>;
using CallbackLoop       = std::function<void()>;

enum class TypeMsg : char {
    UserMsg, Conn, Disconn
};

class Buffer : public __Buffer {
public:
    Buffer(const char *_data, size_t len) : __Buffer(len) {
        memcpy(this->data(), _data, len);
    }
    Buffer(const std::string &str) : __Buffer(str.size()) {
//        std::cout << "string";
        memcpy(this->data(), str.data(), str.size());
        this->operator <<('\0');
    }

    Buffer &operator = (const std::string &str) {
        this->resize(str.size()+1);
        memcpy(this->data(), str.data(), str.size()+1);
        return *this;
    }

    Buffer(size_t sz) : __Buffer(sz) {}
    Buffer() = default;

    template <typename T, typename = typename std::enable_if<std::is_integral<typename std::remove_reference<T>::type>::value>::type>
    Buffer &operator << (const T &obj) {
        for(int i = 0; i < sizeof(obj); ++i)
            push_back(reinterpret_cast<const char *>(&obj)[i]);
        return *this;
    }
    Buffer &operator << (const Buffer &buffer) {
        const size_t sz = buffer.size();
        const char *dt = buffer.data();
        for(int i = 0; i < sz; ++i)
            push_back(dt[i]);
        return *this;
    }

    std::string toString() {

        std::string str;
        const size_t len = size();
        if (len < 1)
            return str;

        str.resize(len-1);

        memcpy(&str[0], data(), len);
//        const char *dt = data();
//        for (int i = 0; i < len; i++)
//            str [i] = dt[i];

        return str;
    }

};

class Device {
    Buffer *_device;
    size_t _pos;
    size_t _sz;
public:
    Device(Buffer *device) : _device(device), _pos(0), _sz(device->size()) {}
    Device(const Device &dev) = default;
    Device &operator = (const Device &) = default;

    size_t read(char *destination, uint64_t len) {
        auto sub = _sz-_pos;
        size_t readlen = sub < len ? sub : len;
        memcpy(destination, _device->data() + _pos, readlen);
        _pos += readlen;
        return readlen;
    }
    size_t getPos() { return _pos; }
    Buffer *getDevice() { return _device; }

    void setBuffer(Buffer *device) {
        _device = device;
        _pos = 0;
        _sz = _device->size();
    }

    void setPos(size_t pos) {
        _pos = pos;
    }

    void reset() {
        _pos = 0;
    }
};

struct SocketFD {
    int _fd;
    operator int&() {
        return _fd;
    }
    SocketFD(const SocketFD &) = default;
    SocketFD() = default;
    SocketFD(int fd) : _fd(fd) {}
    void sendMessage(const std::string &msg) const;
    void sendData(const Buffer &buffer) const;
protected:
    void sendData(const Buffer &buffer, TypeMsg type) const;
    void sendMessage(const std::string &msg, TypeMsg type) const;
};

class TCPSocket : public SocketFD
{
public:
    std::future<void> _fut;
    TCPSocket(const std::string &ip, u_short port);
    virtual ~TCPSocket();

protected:
    void initLoop(CallbackLoop callbackLoop);
    bool readMessage(int fd);
    bool readMessage();

public:
    inline void stop() { _running.store(false); }
    CallbackRecv getCallbackRead() const;
    void setCallbackRead(const CallbackRecv &value);

protected: /* data info to init */
    std::string _ip;
    u_short _port;

protected: /* data read/write buffers */
    CallbackRecv callbackRead = [] (Buffer, SocketFD) {};
    Buffer _buffer;

protected: /* data to init socket */
    std::atomic_bool _running;
    sockaddr_in _sock;

protected:
    void unlockfd();
};

}

#endif // TCPSOCKET_H
