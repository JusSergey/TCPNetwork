#include "filereceiver.h"
#include <fstream>
using namespace Net;

FileReceiver::FileReceiver(const std::string &ip, u_short port)
    : TCPServer(ip, port)
{
    setCallbackRead(_makeCallbackRead());
}

CallbackRead FileReceiver::_makeCallbackRead()
{
    return [this] (Buffer buff, SocketFD socket) -> void {
        std::cout << "file receiver read...\n";

        Net::Device reader(&buff);
        TypeFileMsg type;
        reader.read(&type, sizeof(type));

        switch (type) {
        case TypeFileMsg::CreateFileName:
            receiverFilename = static_cast<const char *>(reader.getCurrentPointerOnData());
            _makeFile(receiverFilename);
            break;
        case TypeFileMsg::Append:
            _readNextPackage(reader);
            break;
        case TypeFileMsg::Complete:
            _callbackFileSendComplete(receiverFilename);
            break;
        }

        _getNextPackage(socket);
    };
}

void FileReceiver::setCallbackFileSendComplete(const CallbackFileSendComplete &callbackFileSendComplete)
{
    _callbackFileSendComplete = callbackFileSendComplete;
}

void FileReceiver::_makeFile(const std::string &filename)
{
    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        std::cout << "can't open file: " << filename << '\n';
    }
    else file.close();
}

void FileReceiver::_readNextPackage(Device &device)
{
    const char *ptrData = static_cast<const char *>(device.getCurrentPointerOnData());
    std::ofstream file(receiverFilename, std::ios::app);
    file.write(ptrData, device.getAvailableSize());
    file.flush();
    file.close();
}

void FileReceiver::_getNextPackage(SocketFD &socket)
{
    Buffer buffGetNext;
    buffGetNext << TypeFileMsg::GetNextPackage;
    socket.sendData(buffGetNext);
}
