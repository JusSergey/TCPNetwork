#include <QCoreApplication>
#include "net.h"
#include <QFile>
#include <chrono>

using namespace Net;

std::function<void(Net::Buffer, Net::SocketFD)>
callbackClientRead() {
    return [] (Net::Buffer buffer, Net::SocketFD socket) {
        std::cout << "client: " << buffer.toString() << '\n';
        std::cout.flush();
        socket.sendMessage((buffer).toString());
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    constexpr const char *IP = "127.0.0.1";
    constexpr u_short PORT = 2114;

    {
        TCPServer server(IP, PORT);
        TCPClient client(IP, PORT);

        server.setCallbackRead(callbackServerRead(server));
        client.setCallbackRead(callbackClientRead());

        server.setCallbackDisconnect([](SocketFD fd){ std::cout << "server: disconnect fd: " << fd << '\n'; });

        client.sendMessage("Hello...");

        std::this_thread::sleep_for(seconds(2));
        client.disconnectFromHost();

        std::cin.get();
        return 0;
    }

    return a.exec();
}
