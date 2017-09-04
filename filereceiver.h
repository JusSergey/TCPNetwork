#ifndef FILERECEIVER_H
#define FILERECEIVER_H
#include "net.h"
#include "file.h"

class FileReceiver : public Net::TCPServer
{
private:
    CallbackFileSendComplete _callbackFileSendComplete = [](const std::string &){};
    Net::Device *dev = nullptr;
    std::string receiverFilename;
public:
    FileReceiver(const std::string &ip, u_short port);

public:
    void setCallbackFileSendComplete(const CallbackFileSendComplete &callbackFileSendComplete);

private:
    Net::CallbackRead _makeCallbackRead();
    void _makeFile(const std::string &filename);
    void _readNextPackage(Net::Device &device);
    void _getNextPackage(Net::SocketFD &socket);
};

#endif // FILERECEIVER_H
