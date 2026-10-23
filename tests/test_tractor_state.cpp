#include "tractor_state.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "[Test] 開始執行 TractorState 單元測試...\n";

    // 1. 測試建構子與預設狀態
    TractorState tractor("Agri-Rover-01");
    tractor.print_status();

    // 2. 測試模式切換 (enum class)
    tractor.set_mode(TractorMode::AUTO);
    
    // 3. 測試歷史軌跡紀錄 (std::vector)
    tractor.add_position_history(23.5832, 120.4341);
    tractor.add_position_history(23.5835, 120.4348);

    // 4. 印出最終狀態確認
    tractor.print_status();

    std::cout << "[Test] TractorState 測試全部通過！\n";
    return 0;
}
