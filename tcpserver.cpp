#include "tcpserver.h"
using namespace Net;

TCPServer::TCPServer(const std::string &ip, u_short port) :
    TCPSocket(ip, port)
{
    if(bind(_fd, (struct sockaddr *)&_sock, sizeof(_sock)) < 0)
        err(0, "bind(): failed.");

    listen(_fd, 16);

    unlockfd();

    initLoop(getCallbackLoopServer());
}

TCPServer::~TCPServer()
{

}

void TCPServer::procSpecifiedMsg(SocketFD &fd)
{
    switch (_typeMsg) {
    case TypeMsg::Disconnect:
        callbackDisconnect(fd);
        fd = SocketStatus::Failed;
        std::cout << "last message: " << _buffer.toString();
        break;
    case TypeMsg::TestConnection: break;
    }
}

void TCPServer::acceptClient()
{
    int fd = accept(_fd, NULL, NULL);
    if (fd >= 0) {
        clientsFD.push_back(fd);
        callbackConnected(fd);
    }
    std::cout.flush();
}

void TCPServer::recvMsg()
{
    for (SocketFD &fd : clientsFD) {
        if (readMessage(fd)) {
            switch (_typeMsg) {
                case TypeMsg::UserMsg:
                    callbackRead(_buffer, fd); break;
                default:
                    procSpecifiedMsg(fd); break;
            }
        }
    }

    clientsFD.remove_if([] (const SocketFD &fd) -> bool { return !fd.isValid(); });
}

CallbackLoop TCPServer::getCallbackLoopServer()
{
    return [this] {
        acceptClient();
        recvMsg();
    };
}

CallbackConnected TCPServer::getCallbackConnected() const
{
    return callbackConnected;
}

void TCPServer::setCallbackConnected(const CallbackConnected &value)
{
    callbackConnected = value;
}

CallbackDisconnect TCPServer::getCallbackDisconnect() const
{
    return callbackDisconnect;
}

void TCPServer::setCallbackDisconnect(const CallbackDisconnect &value)
{
    callbackDisconnect = value;
}

