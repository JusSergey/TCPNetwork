#include "tcpsocket.h"
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
using namespace Net;

TCPSocket::TCPSocket(const std::string &ip, uint16 port) :
    SocketFD(-1),
    _ip(ip),
    _port(port),
    _buffer(MAX_SIZE_PACKET),
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
    /* READ HEAD TCP PACKET */
    uint16 AllBytesSize = 0;
    if (recv(fd, (char*)&AllBytesSize, sizeof(uint16), MSG_PEEK | MSG_DONTWAIT) < sizeof(uint16) || AllBytesSize < sizeof(uint16))
        return false;
    recv(fd, (char*)&AllBytesSize, sizeof(uint16), MSG_DONTWAIT); // IGNORE BYTES

    /* READ TYPE MSG */
    if (recv(fd, (char*)&_typeMsg, sizeof(TypeMsg), MSG_PEEK | MSG_DONTWAIT) < sizeof(TypeMsg))
        return false;
    recv(fd, (char*)&_typeMsg, sizeof(TypeMsg), MSG_DONTWAIT); // IGNORE BYTES

    // set size buffer
    uint16 bufferSize = AllBytesSize - sizeof(uint16) - sizeof(TypeMsg);
    _buffer.resize(bufferSize);

    /* READ BODY MSG */
    if (recv(fd, _buffer.data(), bufferSize, MSG_PEEK | MSG_DONTWAIT) != bufferSize)
        return false;
    recv(fd, _buffer.data(), bufferSize, MSG_DONTWAIT); // IGNORE BYTES

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
    uint16 sz = buffer.size() + sizeof(uint16) + sizeof(type);
    Buffer forSend;
    forSend << sz;
    forSend << type;
    forSend << buffer;
    send(_fd, forSend.data(), forSend.size(), MSG_NOSIGNAL);
}

void SocketFD::sendMessage(const std::string &msg, TypeMsg type) const
{
    sendData(msg, type);
}
