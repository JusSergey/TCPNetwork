#include "filereceiver.h"
#include <fstream>
//using namespace Net;


FileReceiver::FileReceiver(const std::string &ip, u_short port)
    : TCPServer(ip, port)
{
    setCallbackRead(_makeCallbackRead());
}

void FileReceiver::setCallbackAcceptRecvFile(const CallbackAcceptRecvFile &callbackAcceptRecvFile)
{
    _callbackAcceptRecvFile = callbackAcceptRecvFile;
}

CallbackRead FileReceiver::_makeCallbackRead()
{
    return [this] (Buffer buff, SocketFD socket) -> void {

        Device reader(&buff);
        TypeFileMsg type;
        reader.read(&type, sizeof(type));

        bool isNeedNextPackage = true;

        switch (type) {
        case TypeFileMsg::CreateNewFile:
            isNeedNextPackage = _caseCreateNewFile(reader, socket);
            break;
        case TypeFileMsg::Append:
            _caseReadNextPackage(reader);
            break;
        case TypeFileMsg::Complete:
            _callbackFileSendComplete(receiverFilename);
            break;
        }

        if (isNeedNextPackage)
            _caseGetNextPackage(socket);
    };
}

bool FileReceiver::_caseCreateNewFile(Device &reader, SocketFD &socket)
{
    bool isSuccess = true;

    receiverFilename = static_cast<const char *>(reader.getCurrentPointerOnData());
    if (!_callbackAcceptRecvFile(receiverFilename)) {
        isSuccess = false;
        _rejectRecvFile(socket);
    }
    else {
        isSuccess = _makeFile(receiverFilename);
    }

    return isSuccess;
}

void FileReceiver::setCallbackFileSendComplete(const CallbackFileSendComplete &callbackFileSendComplete)
{
    _callbackFileSendComplete = callbackFileSendComplete;
}

bool FileReceiver::_makeFile(const std::string &filename)
{
    bool isSuccess = true;

    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        isSuccess = false;
        std::cout << "can't open file: " << filename << '\n';
    }
    else file.close();

    return isSuccess;
}

void FileReceiver::_caseReadNextPackage(Device &device)
{
    std::cout << "-- recver: " << device.getDevice()->size() << " bytes...\n\n";
    const char *ptrData = static_cast<const char *>(device.getCurrentPointerOnData());
    std::ofstream file(receiverFilename, std::ios::app);

    if (!file.is_open()) {
        std::cout << "failed open file for app [" << receiverFilename << "]\n";
    }
    else {
        file.write(ptrData, device.getAvailableSize());
        file.flush();
    }

    file.close();
}

void FileReceiver::_caseGetNextPackage(SocketFD &socket)
{
    Buffer buffGetNext;
    buffGetNext << TypeFileMsg::GetNextPackage;
    socket.sendData(buffGetNext);
}

void FileReceiver::_rejectRecvFile(SocketFD &socket)
{
    Buffer buffer;
    buffer << TypeFileMsg::RejectRecv;
    socket.sendData(buffer);
}
