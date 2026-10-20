#ifndef TRACTOR_STATE_HPP
#define TRACTOR_STATE_HPP

#include <string>
#include <vector>

// 使用強型別列舉定義拖拉機的運作模式
enum class TractorMode {
    MANUAL,     // 手動控制
    AUTO,       // 自動導航
    ESTOP       // 緊急停止
};

// 儲存位置歷史紀錄的結構體
struct Position {
    double x;
    double y;
};

class TractorState {
public:
    // 建構子：初始化拖拉機名稱與初始狀態
    TractorState(const std::string& robot_name);
    
    // 解構子：用於觀測生命週期與資源釋放
    ~TractorState();

    // 變更運作模式
    void set_mode(TractorMode new_mode);
    
    // 新增目前位置到歷史軌跡紀錄中
    void add_position_history(double x, double y);

    // 唯讀函式 (const)：獲取目前狀態資訊
    void print_status() const;

private:
    std::string name_;                        // 機器人名稱
    TractorMode current_mode_;                // 目前運作模式
    std::vector<Position> position_history_;  // 歷史軌跡數據 (練習 std::vector)
};

#endif // TRACTOR_STATE_HPP

