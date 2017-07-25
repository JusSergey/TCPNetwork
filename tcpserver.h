#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>
#include <string>
#include <list>
#include "tcpsocket.h"

namespace Net {

using CallbackConnected = std::function<void(SocketFD)>;
using CallbackDisconnect= std::function<void(SocketFD)>;

class TCPServer : public TCPSocket
{
public:
    using ListClients = std::list<SocketFD>;

public:
    TCPServer(const std::string &ip, u_short port);
    virtual ~TCPServer();

private:
    void procSpecifiedMsg(SocketFD &fd);
    void acceptClient();
    void recvMsg();
    CallbackLoop getCallbackLoopServer();

public:
    const ListClients &getClients() { return clientsFD; }

    CallbackConnected getCallbackConnected() const;
    void setCallbackConnected(const CallbackConnected &value);

    CallbackDisconnect getCallbackDisconnect() const;
    void setCallbackDisconnect(const CallbackDisconnect &value);

private:
    ListClients clientsFD;
    CallbackConnected callbackConnected = [] (SocketFD) {};
    CallbackDisconnect callbackDisconnect = [] (SocketFD) {};

};

}

#endif // TCPSERVER_H
