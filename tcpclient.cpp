#include "tcpclient.h"
#include <stdlib.h>
using namespace Net;

TCPClient::TCPClient(const std::string &ip, u_short port) :
    TCPSocket(ip, port)
{
    int isConnected = connect(_fd, (sockaddr*)&_sock, sizeof(_sock));
    if (isConnected < 0) {
        err(isConnected, "connect(): failed.");
        exit(-1);
    }

    unlockfd();
    initLoop(getCallbackLoopClient());
}

CallbackLoop TCPClient::getCallbackLoopClient()
{
    return [this] {
        if (readMessage()) {
            callbackRead(_buffer, _fd);
        }
    };
}

void TCPClient::disconnect()
{

}
