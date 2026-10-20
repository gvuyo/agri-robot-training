#include "tractor_state.hpp"
#include <iostream>

// 實作建構子
TractorState::TractorState(const std::string& robot_name) 
    : name_(robot_name), current_mode_(TractorMode::MANUAL) {
    std::cout << "[TractorState] 物件已建構。機器人名稱: " << name_ << " (預設為 MANUAL 模式)\n";
}

// 實作解構子
TractorState::~TractorState() {
    std::cout << "[TractorState] 物件已解構。釋放 " << name_ << " 佔用的資源。\n";
}

// 設定模式
void TractorState::set_mode(TractorMode new_mode) {
    current_mode_ = new_mode;
    std::cout << "[TractorState] 模式已切換。\n";
}

// 紀錄歷史位置
void TractorState::add_position_history(double x, double y) {
    position_history_.push_back(Position{x, y});
    std::cout << "[TractorState] 已更新位置紀錄: (" << x << ", " << y 
              << ")，目前累計點數: " << position_history_.size() << "\n";
}

// 印出當前完整狀態
void TractorState::print_status() const {
    std::string mode_str;
    switch (current_mode_) {
        case TractorMode::MANUAL: mode_str = "MANUAL (手動)"; break;
        case TractorMode::AUTO:   mode_str = "AUTO (自動導航)"; break;
        case TractorMode::ESTOP:  mode_str = "ESTOP (緊急停止)"; break;
    }
    
    std::cout << "\n========================================\n"
              << " 拖拉機名稱: " << name_ << "\n"
              << " 目前模式  : " << mode_str << "\n"
              << " 軌跡點總數: " << position_history_.size() << " 點\n"
              << "========================================\n\n";
}

