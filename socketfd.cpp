#include "socketfd.h"
#include <sys/socket.h>

uint16 SocketFD::sendMessage(const std::string &msg) const
{
    return sendData(Buffer(msg), TypeMsg::UserMsg);
}

uint16 SocketFD::sendData(const Buffer &buffer) const
{
    return sendData(buffer, TypeMsg::UserMsg);
}

uint16 SocketFD::sendMessage(const std::string &msg, TypeMsg type) const
{
    return sendData(msg, type);
}

uint16 SocketFD::sendData(const Buffer &buffer, TypeMsg type) const
{
    const uint16 sz = buffer.size() + sizeof(uint16) + sizeof(type);
    Buffer forSend;
    forSend << sz;
    forSend << type;
    forSend << buffer;

    return send(_fd, forSend.data(), forSend.size(), MSG_NOSIGNAL);
}
