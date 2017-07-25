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
        std::cout << "new client[" << fd << "]\n";
    }
    std::cout.flush();
}

void TCPServer::recvMsg()
{
    for (int fd : clientsFD) {

        if (readMessage(fd)){
            std::cout.flush();
            callbackRead(_buffer, Net::SocketFD(fd));
        }
    }
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

