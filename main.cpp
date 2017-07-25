#include <QCoreApplication>
#include "net.h"
#include <QFile>

using namespace Net;

std::function<void(Net::Buffer, Net::SocketFD)>
callbackClientRead() {
    return [] (Net::Buffer buffer, Net::SocketFD socket) {
        std::cout << "client: " << buffer.toString() << '\n';
        std::cout.flush();
        socket.sendData(buffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    };
}

std::function<void(Net::Buffer, Net::SocketFD)>
callbackServerRead() {
    return [] (Net::Buffer buffer, Net::SocketFD socket) {
        std::cout << "server: " << buffer.toString() << '\n';
        std::cout.flush();
        socket.sendData(buffer);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    };
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    constexpr const char *IP = "127.0.0.1";
    constexpr u_short PORT = 2168;

    {
        TCPServer server(IP, PORT);
        TCPClient client(IP, PORT);

        server.setCallbackConnected([](SocketFD sock){
            sock.sendMessage("Connected success...\n");
        });
        server.setCallbackRead([](Buffer buffer, SocketFD){
            std::cout << "Server: " << buffer.data() << '\n';
        });

        client.sendMessage("Hello, world...");
        client.setCallbackRead([](Buffer buffer, SocketFD) {
            std::cout << "client: " << buffer.toString();
        });
        client.sendMessage("Hello, world...");
        client.sendMessage("Hello, world...");
        client.sendMessage("Hello, world...");
        client.sendMessage("Hello, world...");

        std::cin.get();
        a.exit();
        return 0;
    }

    return a.exec();
}
