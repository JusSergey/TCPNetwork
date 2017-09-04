#ifndef FILESENDER_H
#define FILESENDER_H
#include "net.h"
#include "file.h"

class FileSender : public Net::TCPClient
{
private:
    CallbackFileSendComplete _callbackFileSendComplete = [](const std::string &){};
    Net::Device *readerSendFile = nullptr;
    Net::Device *readerRecvData = nullptr;
    std::string senderFilename;

public:
    FileSender(const std::string &ip, u_short port);

public:
    void asyncSendFile(const std::string &nameOnRemoteMachine,
                       Net::Buffer *ptrToBuffer,
                       const CallbackFileSendComplete &callback = [](const std::string &){});

    void asyncSendFile(const std::string &nameOnRemoteMachine,
                       const std::string &pathToFileInHost,
                       const CallbackFileSendComplete &callback = [](const std::string &){});

private:
    Net::CallbackRead _makeCallbackRead();

private:
    void _sendFilenameOnRemoteMachine(const std::string &filename);
    void _sendNextPacket();
    void _sendSignalComplete();
};

#endif // FILESENDER_H
