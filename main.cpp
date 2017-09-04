//#include <QCoreApplication>
#include "net.h"
//#include <QFile>
#include <chrono>

#include "filesender.h"
#include "filereceiver.h"

using namespace Net;

std::function<void(Net::Buffer, Net::SocketFD)>
callbackClientRead() {
    return [] (Net::Buffer buffer, Net::SocketFD socket) {
        std::cout << "client: " << buffer.toString() << '\n';
        std::cout.flush();
        socket.sendMessage(buffer.toString());
        std::this_thread::sleep_for(milliseconds(500));
    };
}

std::function<void(Net::Buffer, Net::SocketFD)>
callbackServerRead(TCPServer &server) {
    return [&server] (Net::Buffer buffer, Net::SocketFD socket) {
        std::cout << "server: " << buffer.data() << '\n';
        std::cout.flush();
        socket.sendData(buffer);
        std::this_thread::sleep_for(milliseconds(300));
    };
}

void testeDevice() {
    Buffer buff;
    buff << std::string("12345678...");
    Device dev(&buff);
    char *line = new char[buff.size()+1];
    dev.read(line, 4);
    line[4] = '\0';
    (std::cout << "line: " << line).flush();

    dev.read(line, 4);
    line[4] = '\0';
    (std::cout << "line: " << line).flush();
}

int main(int argc, char *argv[])
{
    constexpr const char *IP = "127.0.0.1";
    constexpr u_short PORT = 2122;

    {
        FileReceiver recver(IP, PORT);
        FileSender sender(IP, PORT);
        Buffer buffForSend;

        std::this_thread::sleep_for(seconds(1));
        sender.asyncSendFile("copy.txt", "1.jpg", [](const std::string &filename){
            std::cout << "sending [" << filename << "] complete...\n";
        });
//        sender.asyncSendFile("TextFile.txt", &buffForSend);

//        TCPServer server(IP, PORT);
//        TCPClient client(IP, PORT);

//        server.setCallbackRead(callbackServerRead(server));
//        client.setCallbackRead(callbackClientRead());

//        server.setUserCallbackDisconnect([](SocketFD fd){ std::cout << "server: disconnect fd: " << fd << '\n'; });

//        client.sendMessage("Hello...");

//        std::this_thread::sleep_for(seconds(8));
//        client.disconnectFromHost();

        std::cin.get();
//        return 0;

    }

    return 0;
//    return a.exec();
}
