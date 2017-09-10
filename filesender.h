#ifndef FILESENDER_H
#define FILESENDER_H
#include "net.h"
#include "file.h"

class FileSender : public TCPClient
{
#define DEF_CALLBACK_STR [](const std::string &){}

private:
    CallbackFileSendComplete _callbackFileSendComplete = nullptr;
    CallbackFileSendAbort _callFileSendAbort = nullptr;
    Device *readerSendFile = nullptr;
    Device *readerRecvData = nullptr;
    std::string senderFilename;

public:
    FileSender(const std::string &ip, u_short port);

public:
    void asyncSendFile(const std::string &nameOnRemoteMachine,
                       Buffer *ptrToBuffer,
                       const CallbackFileSendComplete &callback = DEF_CALLBACK_STR,
                       const CallbackFileSendAbort &callbackAbort = DEF_CALLBACK_STR);

    void asyncSendFile(const std::string &nameOnRemoteMachine,
                       const std::string &pathToFileInHost,
                       const CallbackFileSendComplete &callback = DEF_CALLBACK_STR);

private:
    CallbackRead _makeCallbackRead();

private:
    void _sendFilenameOnRemoteMachine(const std::string &filename);
    void _sendNextPacket();
    void _caseRejectRecv();
    void _clearBuffer();
};

#endif // FILESENDER_H
