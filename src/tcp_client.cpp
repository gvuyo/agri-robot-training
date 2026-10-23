#include "tractor_state.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const int PORT = 8080;

class SafeSocket {
public:
    explicit SafeSocket(int fd) : fd_(fd) {}
    ~SafeSocket() {
        if (fd_ >= 0) {
            std::cout << "[RAII] 自動關閉 Client Socket (fd: " << fd_ << ")\n";
            close(fd_);
        }
    }
    SafeSocket(const SafeSocket&) = delete;
    SafeSocket& operator=(const SafeSocket&) = delete;
    int get() const { return fd_; }
private:
    int fd_;
};

int main() {
    std::cout << "[Client] 啟動拖拉機車載通訊端...\n";
    TractorState client_tractor("Tractor-ClientNode");

    SafeSocket client_socket(socket(AF_INET, SOCK_STREAM, 0));
    if (client_socket.get() < 0) {
        std::perror("Socket 建立失敗");
        return 1;
    }

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::perror("不合法的地址");
        return 1;
    }

    if (connect(client_socket.get(), (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::perror("連線失敗");
        return 1;
    }
    std::cout << "[Client] 成功連線至 Gateway 伺服器！\n";

    std::vector<std::string> test_messages = {
        "Tractor Status OK\n",
        "Requesting AUTO mode navigation\n"
    };

    char buffer[1024] = {0};
    for (const auto& msg : test_messages) {
        std::cout << "[Client] 發送: " << msg;
        send(client_socket.get(), msg.c_str(), msg.length(), 0);

        std::memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(client_socket.get(), buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            std::cout << "[Client] 接收回傳: " << buffer;
        }
        sleep(1);
    }

    return 0; // main 結束，client_socket 離開範疇，自動解構釋放 fd！
}

