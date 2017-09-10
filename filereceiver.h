#ifndef FILERECEIVER_H
#define FILERECEIVER_H
#include "net.h"
#include "file.h"

class FileReceiver : public TCPServer
{
private:
    CallbackFileSendComplete _callbackFileSendComplete = [](const std::string &){};
    CallbackAcceptRecvFile _callbackAcceptRecvFile = [](const std::string &) { return true; };
    Device *dev = nullptr;
    std::string receiverFilename;

public:
    FileReceiver(const std::string &ip, u_short port);

public:
    void setCallbackFileSendComplete(const CallbackFileSendComplete &callbackFileSendComplete);
    void setCallbackAcceptRecvFile(const CallbackAcceptRecvFile &callbackAcceptRecvFile);

private:
    CallbackRead _makeCallbackRead();
    bool _caseCreateNewFile(Device &reader, SocketFD &socket); // return - is success
    void _caseReadNextPackage(Device &device);
    void _caseGetNextPackage(SocketFD &socket);
    bool _makeFile(const std::string &filename);
    void _rejectRecvFile(SocketFD &socket);
};

#endif // FILERECEIVER_H
