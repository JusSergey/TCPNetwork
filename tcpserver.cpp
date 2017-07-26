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
    for (SocketFD &fd : clientsFD)
        if (readMessage(fd))
            _callbackRead(_buffer, fd);

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

void TCPServer::disconnectClientFromServer(SocketFD fdClient)
{
    fdClient.sendMessage("DISCONNECT FROM HOST", TypeMsg::Disconnect);

    for (SocketFD &fd : clientsFD)
        if (fd == fdClient)
            fd.setValidFalse();
}

void TCPServer::specifiedConfirmConnection(Buffer &buff, SocketFD &socket)
{
}

void TCPServer::specifiedDisconnect(Buffer &buff, SocketFD &socket)
{
    socket.closeSocketFD();
}

void TCPServer::specifiedTectConnection(Buffer &buff, SocketFD &socket)
{
    socket.sendMessage("", TypeMsg::ConfirmConnection);
}

