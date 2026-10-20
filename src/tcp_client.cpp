#include "tractor_state.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const int PORT = 8080;

int main() {
    std::cout << "[Client] 啟動拖拉機車載通訊端...\n";
    TractorState client_tractor("Tractor-ClientNode");

    // 1. 建立 Socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::perror("Socket 建立失敗");
        return 1;
    }

    // 2. 設定伺服器地址 (127.0.0.1 本地端)
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::perror("不合法的地址/不支援的地址家族");
        close(sock_fd);
        return 1;
    }

    // 3. 連線至 Gateway 伺服器 (Connect)
    if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::perror("連線失敗");
        close(sock_fd);
        return 1;
    }
    std::cout << "[Client] 成功連線至 Gateway 伺服器！\n";

    // 4. 發送測試資料並等待 Echo 回傳
    std::vector<std::string> test_messages = {
        "Tractor Status OK\n",
        "Requesting AUTO mode navigation\n",
        "Current Position X:10.5 Y:20.3\n"
    };

    char buffer[1024] = {0};
    for (const auto& msg : test_messages) {
        std::cout << "[Client] 發送: " << msg;
        send(sock_fd, msg.c_str(), msg.length(), 0);

        std::memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_received = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            std::cout << "[Client] 接收回傳: " << buffer;
        }
        sleep(1); // 停頓一秒再發下一條
    }

    // 5. 關閉 Socket
    close(sock_fd);
    std::cout << "[Client] 通訊結束，關閉連線。\n";
    return 0;
}
