#ifndef SOCKETFD_H
#define SOCKETFD_H

#include <unistd.h>
#include "buffer.h"
#include "typedefs.h"

enum class SocketStatus : int {
    Failed = -1
};
enum class TypeMsg : char {
    UserMsg, TestConnection, Disconnect, ConfirmConnection
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

    uint16 sendMessage(const std::string &msg) const;
    uint16 sendData(const Buffer &buffer) const;
protected:
    uint16 sendData(const Buffer &buffer, TypeMsg type) const;
    uint16 sendMessage(const std::string &msg, TypeMsg type) const;
};

#endif // SOCKETFD_H
