#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <err.h>
#include <cstring>

#include <functional>
#include <future>
#include <iostream>
#include <vector>
#include <type_traits>

namespace Net {

using namespace std::chrono;

class TCPServer;
class Buffer;
struct SocketFD;

using __Buffer     = std::vector<char>;
using CallbackRead = std::function<void(Buffer, SocketFD)>;
using CallbackLoop = std::function<void()>;

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;

using int64 = int64_t;
using int32 = int32_t;
using int16 = int16_t;

enum class TypeMsg : char {
    UserMsg, TestConnection, Disconnect, ConfirmConnection
};

enum class SocketStatus : int {
    Failed = -1
};

constexpr auto MAX_SIZE_PACKET = 65535 - sizeof(uint16) - sizeof(TypeMsg);
constexpr auto DELAY_TIME = 1; //ms

class Buffer : public __Buffer {
public:
    Buffer(const char *_data, size_t len) : __Buffer(len) {
        memcpy(this->data(), _data, len);
    }
    Buffer(const std::string &str) : __Buffer(str.size()+1) {
        auto strsz = str.size();
        memcpy(this->data(), str.data(), strsz);
        data()[strsz] = '\0';
    }
    Buffer &operator = (const std::string &str) {
        this->resize(str.size());
        memcpy(this->data(), str.data(), str.size());
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

    Buffer &operator << (const std::string &str) {
        const size_t sz = str.size();
        const char *dt = str.data();
        reserve(size() + sz + 4);
        for(int i = 0; i < sz; ++i)
            push_back(dt[i]);
        return *this;
    }

    Buffer &operator << (const Buffer &buffer) {
        const size_t sz = buffer.size();
        const char *dt = buffer.data();
        reserve(size() + sz + 4);
        for(int i = 0; i < sz; ++i)
            push_back(dt[i]);
        return *this;
    }

    void append(void *_src, size_t len) {
        reserve(size() + len + 4);
        for (size_t i = 0; i < len; ++i)
            push_back(static_cast<const char*>(_src)[i]);
    }

    template <typename EnumT, typename = typename std::enable_if<std::is_enum<EnumT>::value>::type>
    Buffer &operator << (EnumT type) {
        return this->operator << (static_cast<typename std::underlying_type<EnumT>::type>(type));
    }

    std::string toString() const {

        std::string str;
        const size_t len = size();
        if (len < 1)
            return str;

        str.resize(len-1);

        memcpy(&str[0], data(), len);

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

    size_t read(void *destination, uint64 len) {
        auto sub = _sz-_pos;
        size_t readlen = sub < len ? sub : len;
        memcpy(destination, _device->data() + _pos, readlen);
        _pos += readlen;
        return readlen;
    }
    inline size_t getPos() const { return _pos; }
    inline size_t getAvailableSize() const { return _device->size() - _pos; }
    inline Buffer *getDevice() const { return _device; }
    inline void *getCurrentPointerOnData() const { return _device->data() + _pos; }

    void setBuffer(Buffer *device) {
        _device = device;
        _pos = 0;
        _sz = _device->size();
    }

    inline void setPos(size_t pos) { _pos = pos; }

    inline void addPos(size_t offset) { _pos += offset; }

    inline void reset() { _pos = 0; }
};

struct SocketFD {
    friend class TCPServer;
    friend class TCPClient;

    int _fd;
    operator int&() {
        return _fd;
    }
    SocketFD &operator = (const SocketStatus &status) {
        _fd = static_cast<std::underlying_type<SocketStatus>::type>(status);
        return *this;
    }

    inline int toInt() const { return _fd; }

    SocketFD(const SocketFD &) = default;
    SocketFD(int fd = -1) : _fd(fd) {}

    inline bool operator == (const SocketFD &_cmp) const { return _fd == _cmp._fd; }
    inline bool operator != (const SocketFD &_cmp) const { return _fd != _cmp._fd; }
    inline bool operator <  (const SocketFD &_cmp) const { return _fd <  _cmp._fd; }

    inline bool operator == (const SocketStatus &_cmp) const
    { return _fd == static_cast<std::underlying_type<SocketStatus>::type>(_cmp); }
    inline bool operator != (const SocketStatus &_cmp) const
    { return _fd != static_cast<std::underlying_type<SocketStatus>::type>(_cmp); }

    inline bool isValid() const { return *this != SocketStatus::Failed; }
    inline void setValidFalse() { _fd = static_cast<std::underlying_type<SocketStatus>::type>(SocketStatus::Failed); }
    inline void closeSocketFD() { close(_fd); setValidFalse(); }

    void sendTestConnection() const;

    ///
    /// \brief send functions
    /// \param msg/buffer
    /// \return sendlen
    ///
    uint16 sendMessage(const std::string &msg) const;
    uint16 sendData(const Buffer &buffer) const;
protected:
    uint16 sendData(const Buffer &buffer, TypeMsg type) const;
    uint16 sendMessage(const std::string &msg, TypeMsg type) const;
};

class TCPSocket : public SocketFD
{
public:
    std::future<void> _fut;
    TCPSocket(const std::string &ip, uint16 port);
    virtual ~TCPSocket();

protected:
    void initLoop(CallbackLoop callbackLoop);
    bool readMessage(SocketFD &fd);
    bool readMessage();

public:
    void stop();
    CallbackRead getCallbackRead() const;
    void setCallbackRead(const CallbackRead &value);
    inline int64 getTimeConfirmConnectionMSEC() const {
        return (duration_cast<milliseconds>(steady_clock::now() - _lastConfirmConnection)).count();
    }
    template <typename Enum, typename EnumType = typename std::underlying_type<Enum>::type>
    constexpr inline EnumType toUnderlying(Enum value) const {
        return static_cast<typename std::underlying_type<Enum>::type>(value);
    }

protected: /* data info to init */
    std::string _ip;
    u_short _port;

protected: /* data read/write buffers */
    CallbackRead _callbackRead = [] (Buffer, SocketFD) {};
    Buffer _buffer;
    TypeMsg _typeMsg;
    time_point<steady_clock> _lastConfirmConnection;

protected: /* data to init socket */
    std::atomic_bool _running;
    sockaddr_in _sock;

protected:
    void unlockfd();

protected:
    virtual void specifiedDisconnect(Buffer &buff, SocketFD &socket) = 0;
    virtual void specifiedTectConnection(Buffer &buff, SocketFD &socket) = 0;
    virtual void specifiedConfirmConnection(Buffer &buff, SocketFD &socket) = 0;

private:
    void procSpecifiedMsg(Buffer &buff, SocketFD &socket);
};

}

#endif // TCPSOCKET_H
