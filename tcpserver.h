#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <functional>
#include <string>
#include <list>
#include "tcpsocket.h"
#include "socketfd.h"
#include "callbacktypes.h"

//namespace Net {

class TCPServer : public TCPSocket
{
public:
    using ListClients = std::list<SocketFD>;

public:
    TCPServer(const std::string &ip, u_short port);
    virtual ~TCPServer();

private:
    void acceptClient();
    void recvMsg();
    CallbackLoop getCallbackLoopServer();

public:
    inline const ListClients &getClients() const { return _clientsFD; }

    CallbackConnected getUserCallbackConnected() const;
    void setUserCallbackConnected(const CallbackConnected &value);

    CallbackDisconnect getUserCallbackDisconnect() const;
    void setUserCallbackDisconnect(const CallbackDisconnect &value);

    void disconnectClientFromServer(SocketFD fdClient);

protected:
    int _serverFD;
    ListClients _clientsFD;
    CallbackConnected _userCallbackConnected = [] (SocketFD) {};
    CallbackDisconnect _userCallbackDisconnect = [] (SocketFD) {};

protected:
    virtual void specifiedDisconnect(Buffer &buff, SocketFD &socket) override;
    virtual void specifiedTectConnection(Buffer &buff, SocketFD &socket) override;
    virtual void specifiedConfirmConnection(Buffer &buff, SocketFD &socket) override;

    virtual void postConnected(SocketFD) {}
    virtual void prevDisconnected(SocketFD) {}
};

//}

#endif // TCPSERVER_H
