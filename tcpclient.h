#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <string>
#include "tcpsocket.h"

namespace Net {

class TCPClient : public TCPSocket
{
public:
    TCPClient(const std::string &ip, u_short port);
    CallbackLoop getCallbackLoopClient();
    void disconnect();
};

}

#endif // TCPCLIENT_H
