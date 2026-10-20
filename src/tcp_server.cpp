#include "tractor_state.hpp"
#include <iostream>

int main() {
    std::cout << "--- 進入 main() 函式 ---\n";

    // 1. 建立一個拖拉機狀態物件（呼叫建構子）
    TractorState my_tractor("AgriBot-V1");

    // 2. 操縱狀態與寫入歷史紀錄
    my_tractor.print_status();
    
    my_tractor.add_position_history(120.34, 23.56);
    my_tractor.add_position_history(120.35, 23.57);
    
    // 3. 切換模式至自動導航
    my_tractor.set_mode(TractorMode::AUTO);
    my_tractor.print_status();

    std::cout << "--- 準備離開 main() 函式 ---\n";
    // 當 main 結束時，my_tractor 離開生存空間 (Scope)，將自動觸發解構子！
    return 0;
}

