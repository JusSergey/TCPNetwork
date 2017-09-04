#include "tcpsocket.h"
#include <cstring>
#include <fcntl.h>
#include <thread>
#include <unistd.h>
using namespace Net;

TCPSocket::TCPSocket(const std::string &ip, uint16 port) :
    SocketFD(-1),
    _ip(ip),
    _port(port),
    _buffer(MAX_SIZE_PACKET),
    _running(true),
    _lastConfirmConnection(steady_clock::now())
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
    stop();
}

void TCPSocket::initLoop(CallbackLoop callbackLoop)
{
    _fut = std::async(std::launch::async, [&, callbackLoop] {
        while (_running.load()) {
            callbackLoop();
            std::this_thread::sleep_for(milliseconds(DELAY_TIME));
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

void TCPSocket::procSpecifiedMsg(Buffer &buff, SocketFD &socket)
{
    switch (_typeMsg) {
    case TypeMsg::ConfirmConnection: specifiedConfirmConnection(buff, socket); break;
    case TypeMsg::Disconnect: specifiedDisconnect(buff, socket); break;
    case TypeMsg::TestConnection: specifiedTectConnection(buff, socket); break;
    default: (std::cout << "TCPSocket::procSpecifiedMsg():terminate").flush(); std::terminate();
    }
}

bool TCPSocket::readMessage(SocketFD &fd)
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

    auto callbackReadBody = [&] (Buffer &buff) -> bool {
        // set size buffer
        uint16 bufferSize = AllBytesSize - sizeof(uint16) - sizeof(TypeMsg);
        buff.resize(bufferSize);

        if (recv(fd, buff.data(), bufferSize, MSG_PEEK | MSG_DONTWAIT) != bufferSize)
            return false;

        recv(fd, buff.data(), bufferSize, MSG_DONTWAIT); // IGNORE BYTES
        _lastConfirmConnection = steady_clock::now();
        return true;
    };

    // if specified msg
    if (_typeMsg != TypeMsg::UserMsg) {
        Buffer b;
        callbackReadBody(b);
        procSpecifiedMsg(b, fd);
        return false;
    }

    return callbackReadBody(_buffer);
}

bool TCPSocket::readMessage()
{
    return readMessage(*this);
}

void TCPSocket::stop() {
    if (_running.load()){
        _running.store(false);
        closeSocketFD();
    }
}

CallbackRead TCPSocket::getCallbackRead() const
{
    return _callbackRead;
}

void TCPSocket::setCallbackRead(const CallbackRead &value)
{
    _callbackRead = value;
}

//////////////////////////
///  SocketFD Implement.
///  S. J.

void SocketFD::sendTestConnection() const
{
    sendMessage("", TypeMsg::TestConnection);
}

uint16 SocketFD::sendMessage(const std::string &msg) const
{
    return sendData(Buffer(msg), TypeMsg::UserMsg);
}

uint16 SocketFD::sendData(const Buffer &buffer) const
{
    return sendData(buffer, TypeMsg::UserMsg);
}

uint16 SocketFD::sendData(const Buffer &buffer, TypeMsg type) const
{
    uint16 sz = buffer.size() + sizeof(uint16) + sizeof(type);
    Buffer forSend;
    forSend << sz;
    forSend << type;
    forSend << buffer;
    return send(_fd, forSend.data(), forSend.size(), MSG_NOSIGNAL);
}

uint16 SocketFD::sendMessage(const std::string &msg, TypeMsg type) const
{
    return sendData(msg, type);
}
