#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include "tcpsocket.h"

namespace Net {

class TCPClient : public TCPSocket
{
public:
    TCPClient(const std::string &ip, u_short port);
    virtual ~TCPClient();
    CallbackLoop getCallbackLoopClient();
    void disconnectFromHost();

protected:
    virtual void specifiedDisconnect(Buffer &buff, SocketFD &socket) override;
    virtual void specifiedTectConnection(Buffer &buff, SocketFD &socket) override;
    virtual void specifiedConfirmConnection(Buffer &buff, SocketFD &socket) override;
};

}

#endif // TCPCLIENT_H
