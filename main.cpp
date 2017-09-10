#include "net.h"
#include "buffer.h"
#include "device.h"
#include "socketfd.h"
#include <chrono>

#include "filesender.h"
#include "filereceiver.h"

using namespace std;

CallbackAcceptRecvFile makeAcceptRecvFile() {
    return [] (const std::string &name) -> bool {
        std::cout << "accept file: " << name << " ?\n";
        int n;
        std::cin >> n;
        return n;
    };
}

int main()
{
    constexpr const char *IP = "127.0.0.1";
    constexpr u_short PORT = 2124;

    {
        FileReceiver recver(IP, PORT); recver.setName("recver");
        FileSender sender(IP, PORT); sender.setName("sender");

        recver.setCallbackAcceptRecvFile(makeAcceptRecvFile());

        std::this_thread::sleep_for(std::chrono::seconds(1));
        sender.asyncSendFile("copy.png", "1.mp3", [&](const std::string &filename) {
            std::cout << "sending [" << filename << "] complete...\n";
            sender.stop();
        });

        while (sender.isRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    }

    return 0;
}
