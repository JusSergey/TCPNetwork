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
    friend struct SocketFD;
    using ListClients = std::list<SocketFD>;

public:
    TCPServer(const std::string &ip, u_short port);
    virtual ~TCPServer();

private:
    void acceptClient();
    void recvMsg();
    CallbackLoop getCallbackLoopServer();

public:
    inline const ListClients &getClients() { return clientsFD; }

    CallbackConnected getCallbackConnected() const;
    void setCallbackConnected(const CallbackConnected &value);

    CallbackDisconnect getCallbackDisconnect() const;
    void setCallbackDisconnect(const CallbackDisconnect &value);

    void disconnectClientFromServer(SocketFD fdClient);

private:
    ListClients clientsFD;
    CallbackConnected callbackConnected = [] (SocketFD) {};
    CallbackDisconnect callbackDisconnect = [] (SocketFD) {};

protected:
    virtual void specifiedDisconnect(Buffer &buff, SocketFD &socket) override;
    virtual void specifiedTectConnection(Buffer &buff, SocketFD &socket) override;
    virtual void specifiedConfirmConnection(Buffer &buff, SocketFD &socket) override;

};

}

#endif // TCPSERVER_H
