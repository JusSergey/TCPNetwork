#include "tcpserver.h"
//using namespace Net;

TCPServer::TCPServer(const std::string &ip, u_short port) :
    TCPSocket(ip, port)
{
    if((_serverFD = bind(_fd, (struct sockaddr *)&_sock, sizeof(_sock))) < 0)
        err(0, "bind(): failed.");

    listen(_fd, 8);

    unlockfd();

    initLoop(getCallbackLoopServer());
}

TCPServer::~TCPServer()
{
    for (SocketFD &socket : _clientsFD)
        disconnectClientFromServer(socket);
    close(_serverFD);
}

void TCPServer::acceptClient()
{
    int fd = accept(_fd, NULL, NULL);

    if (fd >= 0) {
        _clientsFD.push_back(fd);
        postConnected(SocketFD(fd));
        _userCallbackConnected(fd);
    }
    std::cout.flush();
}

void TCPServer::recvMsg()
{
    for (SocketFD &fd : _clientsFD)
        if (readMessage(fd))
            _callbackRead(_buffer, fd);

    _clientsFD.remove_if([] (const SocketFD &fd) -> bool { return !fd.isValid(); });
}

CallbackLoop TCPServer::getCallbackLoopServer()
{
    return [this] {
        acceptClient();
        recvMsg();
    };
}

CallbackConnected TCPServer::getUserCallbackConnected() const
{
    return _userCallbackConnected;
}

void TCPServer::setUserCallbackConnected(const CallbackConnected &value)
{
    _userCallbackConnected = value;
}

CallbackDisconnect TCPServer::getUserCallbackDisconnect() const
{
    return _userCallbackDisconnect;
}

void TCPServer::setUserCallbackDisconnect(const CallbackDisconnect &value)
{
    _userCallbackDisconnect = value;
}

void TCPServer::disconnectClientFromServer(SocketFD fdClient)
{
    fdClient.sendMessage("DISCONNECT FROM HOST", TypeMsg::Disconnect);

    for (SocketFD &fd : _clientsFD)
        if (fd == fdClient) {
            std::cout.flush();
            fd.setValidFalse();
            break;
        }

    prevDisconnected(fdClient);
    _userCallbackDisconnect(fdClient);
}

void TCPServer::specifiedConfirmConnection(Buffer &buff, SocketFD &socket)
{
}

void TCPServer::specifiedDisconnect(Buffer &buff, SocketFD &socket)
{
    prevDisconnected(socket);
    socket.closeSocketFD();
    _userCallbackDisconnect(socket);
}

void TCPServer::specifiedTectConnection(Buffer &buff, SocketFD &socket)
{
    socket.sendMessage("", TypeMsg::ConfirmConnection);
}

