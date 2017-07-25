#include "tcpsocket.h"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
using namespace Net;

TCPSocket::TCPSocket(const std::string &ip, u_short port) :
    SocketFD(-1),
    _ip(ip),
    _port(port),
    _buffer(_64KB),
    _running(true)
{
    memset(&_sock, '\0', sizeof(_sock));

    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd < 0)
        err(_fd, "socket()");

    _sock.sin_family = AF_INET;
    _sock.sin_port = htons(_port);
    _sock.sin_addr.s_addr = inet_addr(_ip.c_str());
}

TCPSocket::~TCPSocket()
{
    if (_running.load())
        stop();

    close(_fd);
    std::cout.flush();
}

void TCPSocket::initLoop(CallbackLoop callbackLoop)
{
    _fut = std::async(std::launch::async, [&, callbackLoop] {
        while (_running.load()) {
            callbackLoop();
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME));
        }
    });
}

void TCPSocket::unlockfd()
{
    int f;

    if ((f = fcntl(_fd, F_GETFL, 0x00000000000)) == -1 ||
             fcntl(_fd, F_SETFL, f | O_NONBLOCK) == -1)
    {
        err(1, "fcntl()");
    }
}

bool TCPSocket::readMessage(int fd)
{
    u_short datalen = 0;
    ssize_t readlen = recv(fd, (char*)&datalen, sizeof(u_short), MSG_PEEK | MSG_DONTWAIT);
    if (readlen < 2 || datalen < 2) {
        return false;
    }

    u_short ignore = 0;
    if (recv(fd, (char*)&ignore, sizeof(u_short), MSG_DONTWAIT) < 2)
        return false;

    _buffer.resize(datalen-sizeof(u_short));

    readlen = recv(fd, _buffer.data(), datalen - sizeof(u_short), MSG_PEEK | MSG_DONTWAIT);
    if (readlen != (datalen-sizeof(u_short)))
        return false;

    recv(fd, _buffer.data(), datalen - sizeof(u_short), MSG_DONTWAIT);

    return true;
}

bool TCPSocket::readMessage()
{
    return readMessage(_fd);
}

CallbackRecv TCPSocket::getCallbackRead() const
{
    return callbackRead;
}

void TCPSocket::setCallbackRead(const CallbackRecv &value)
{
    callbackRead = value;
}

//////////////////////////
///  SocketFD Implement.
///  S. J.

void SocketFD::sendMessage(const std::string &msg) const
{
    sendData(Buffer(msg), TypeMsg::UserMsg);
}

void SocketFD::sendData(const Buffer &buffer) const
{
    sendData(buffer, TypeMsg::UserMsg);
}

void SocketFD::sendData(const Buffer &buffer, TypeMsg type) const
{
    u_short sz = buffer.size() + sizeof(u_short);
    Buffer forSend;
    forSend << sz;
    forSend << buffer;
    send(_fd, forSend.data(), forSend.size(), MSG_NOSIGNAL);
}

void SocketFD::sendMessage(const std::string &msg, TypeMsg type) const
{
    sendData(msg, type);
}
