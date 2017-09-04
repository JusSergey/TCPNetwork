#include "filesender.h"
#include <fstream>
using namespace Net;

FileSender::FileSender(const std::string &ip, u_short port)
    : TCPClient(ip, port)
{
    setCallbackRead(_makeCallbackRead());
}

void FileSender::asyncSendFile(const std::string &nameOnRemoteMachine, Buffer *ptrToBuffer, const CallbackFileSendComplete &callback)
{
    _callbackFileSendComplete = callback;
    if (readerSendFile)
        delete readerSendFile;
    readerSendFile = new Net::Device(ptrToBuffer);
    _sendFilenameOnRemoteMachine(nameOnRemoteMachine);
}

void FileSender::asyncSendFile(const std::string &nameOnRemoteMachine, const std::string &pathToFileInHost, const CallbackFileSendComplete &callback)
{
    senderFilename = pathToFileInHost;

    std::ifstream file(senderFilename, std::ios::in);
(std::cout << "open").flush();
    if (!file.is_open()) {
        std::cout << "can't open file: " << senderFilename << '\n';
        return;
    }

    file.seekg (0, std::ios::end);
    auto size = file.tellg();
    file.seekg(0);

    Buffer *buff = new Buffer(size);
    file.read(buff->data(), size);

    asyncSendFile(nameOnRemoteMachine, buff, [=](const std::string &str) {
        callback(str);
        delete buff;
    });
}

CallbackRead FileSender::_makeCallbackRead()
{
    return [this] (Buffer buff, SocketFD sock) -> void {
        std::cout << "file sender read...\n";
        Net::Device reader(&buff);
        TypeFileMsg type;
        reader.read(&type, sizeof(type));
        switch (type) {
        case TypeFileMsg::GetNextPackage:
            _sendNextPacket();
            break;
        }
    };
}

void FileSender::_sendFilenameOnRemoteMachine(const std::string &filename)
{
    Buffer bufferFilenameMsg;
    bufferFilenameMsg << TypeFileMsg::CreateFileName;
    bufferFilenameMsg << filename;
    sendData(bufferFilenameMsg);
}

void FileSender::_sendNextPacket()
{
    const size_t availableBytes = readerSendFile->getAvailableSize();

    if (availableBytes > 0) {

        Buffer buff;
        buff << TypeFileMsg::Append;

        constexpr auto MAX_BUFFER = MAX_SIZE_PACKET - sizeof(TypeFileMsg);
        const size_t sizeToSend = availableBytes > MAX_BUFFER ? MAX_BUFFER : availableBytes;

        buff.append(readerSendFile->getCurrentPointerOnData(), sizeToSend);

        readerSendFile->addPos(buff.size()-sizeof(TypeFileMsg));
        sendData(buff);
        std::cout << "-- receiver send " << buff.size() << " bytes...\n";
    }
    else {
        _callbackFileSendComplete(senderFilename);
    }
}

void FileSender::_sendSignalComplete()
{

}
