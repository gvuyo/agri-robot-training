#include "tractor_state.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;

int main() {
    std::cout << "[Server] 啟動農業機器人 Gateway 監聽服務...\n";
    TractorState tractor("AgriServer-Gateway");

    // 1. 建立 Socket descriptor
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::perror("Socket 建立失敗");
        return 1;
    }

    // 解決 Address already in use 問題，允許重複繫結
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. 綁定 IP 與 Port (Bind)
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // 監聽所有網路介面
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::perror("Bind 失敗");
        close(server_fd);
        return 1;
    }

    // 3. 開始監聽 (Listen)
    if (listen(server_fd, 3) < 0) {
        std::perror("Listen 失敗");
        close(server_fd);
        return 1;
    }
    std::cout << "[Server] 正在通訊埠 " << PORT << " 上等待機器人連線...\n";

    // 4. 接受連線 (Accept)
    struct sockaddr_in client_addr;
    socklen_t addrlen = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addrlen);
    if (client_fd < 0) {
        std::perror("Accept 失敗");
        close(server_fd);
        return 1;
    }
    std::cout << "[Server] 機器人已成功連線！開始進行 Echo 測試...\n";

    // 5. 讀取並回傳資料 (Echo Loop)
    char buffer[1024] = {0};
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_read <= 0) {
            std::cout << "[Server] 客戶端中斷連線或讀取錯誤。\n";
            break;
        }

        std::cout << "[Server] 收到資料: " << buffer;
        
        // 模擬修改拖拉機狀態：若收到特徵字串則改變模式
        if (std::string(buffer).find("AUTO") != std::string::npos) {
            tractor.set_mode(TractorMode::AUTO);
        }

        // 把資料原封不動丟回去 (Echo)
        send(client_fd, buffer, bytes_read, 0);
    }

    // 6. 清理關閉資源
    close(client_fd);
    close(server_fd);
    std::cout << "[Server] 服務關閉。\n";
    return 0;
}

