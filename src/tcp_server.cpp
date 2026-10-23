#include "tractor_state.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int PORT = 8080;

// Day 4 核心：實作 Socket Descriptor 的 RAII 資源管理器
class SafeSocket {
public:
    explicit SafeSocket(int fd) : fd_(fd) {}
    
    // 解構子：當 SafeSocket 離開 scope 時，自動進行資源清理
    ~SafeSocket() {
        if (fd_ >= 0) {
            std::cout << "[RAII] 自動釋放 Socket Descriptor (fd: " << fd_ << ")\n";
            close(fd_);
        }
    }

    // 禁止複製 (Copy)，防止同一個 fd 被 close 兩次
    SafeSocket(const SafeSocket&) = delete;
    SafeSocket& operator=(const SafeSocket&) = delete;

    // 允許轉移擁有權 (Move)
    SafeSocket(SafeSocket&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }

    int get() const { return fd_; }
    void reset() { if (fd_ >= 0) { close(fd_); fd_ = -1; } }

private:
    int fd_;
};

int main() {
    std::cout << "[Server] 啟動農業機器人 Gateway 監聽服務 (RAII 重構版)...\n";
    TractorState tractor("AgriServer-Gateway");

    // 使用 RAII 管理監聽端的 Socket
    SafeSocket server_socket(socket(AF_INET, SOCK_STREAM, 0));
    if (server_socket.get() < 0) {
        std::perror("Socket 建立失敗");
        return 1;
    }

    int opt = 1;
    setsockopt(server_socket.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_socket.get(), (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::perror("Bind 失敗");
        return 1;
    }

    if (listen(server_socket.get(), 3) < 0) {
        std::perror("Listen 失敗");
        return 1;
    }

    // 無限循環：允許 Client 斷線後，Server 繼續接受下一次連線而不 Crash
    while (true) {
        std::cout << "\n[Server] 正在通訊埠 " << PORT << " 上等待機器人連線...\n";

        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        
        int incoming_fd = accept(server_socket.get(), (struct sockaddr*)&client_addr, &addrlen);
        if (incoming_fd < 0) {
            std::perror("Accept 失敗");
            continue; // 監聽失敗則等待下一次
        }

        // 用內層的 Scope 加上 RAII 來管理客戶端連線生命週期
        {
            SafeSocket client_socket(incoming_fd);
            std::cout << "[Server] 機器人已成功連線 (fd: " << client_socket.get() << ")！\n";

            char buffer[1024] = {0};
            while (true) {
                std::memset(buffer, 0, sizeof(buffer));
                ssize_t bytes_read = recv(client_socket.get(), buffer, sizeof(buffer) - 1, 0);
                
                if (bytes_read <= 0) {
                    std::cout << "[Server] 客戶端中斷連線。即將離開 Scope 並觸發 RAII 清理...\n";
                    break; // 離開內層迴圈
                }

                std::cout << "[Server] 收到資料: " << buffer;
                
                if (std::string(buffer).find("AUTO") != std::string::npos) {
                    tractor.set_mode(TractorMode::AUTO);
                }

                send(client_socket.get(), buffer, bytes_read, 0);
            }
        } // <--- client_socket 在這裡離開 Scope，解構子會被自動呼叫，自動 close(client_fd)！
        
        std::cout << "[Server] 成功釋放目前連線資源，準備迎接下一個客戶端。\n";
    }

    return 0;
}

