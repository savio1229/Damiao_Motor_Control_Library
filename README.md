# Damiao Motor Control Library

本函式庫專為 Arduino 平台開發，旨在透過 USB Host Shield 的 ACM 介面封裝 USB2CAN 協定，實現對达妙（Damiao）馬達的精準控制。

本庫採用統一的**單一萬能控制器架構**，同時支援**速度模式（Velocity Mode）**與**位置速度模式（Position-Velocity Mode）**。開發者可以使用同一個控制器實例，在同一個硬體通訊管道下對多個不同模式的大廟馬達進行獨立、同步的混合控制，完美避免了通訊埠搶佔與底層硬體重複初始化的問題。

---

## 1. 專案名稱與簡介

* **函式庫名稱**：Damiao Motor Control Library
* **主要用途**：本函式庫封裝了达妙馬達的 USB2CAN 通訊協定。開發者只需透過 USB Host Shield 擴充板的 ACM 介面，即可輕鬆地在 Arduino 上對多個达妙馬達進行獨立或同步的使能、失能、速度控制與位置控制。

---

## 2. 安裝教學

1. **下載原始碼**：將本函式庫的原始碼下載並打包為 `.zip` 檔案，或確保資料夾名稱為 `Damiao_Motor_Control_Library`。
2. **匯入 Arduino IDE**：
   * 開啟 Arduino IDE，點選選單中的 `草稿碼` (Sketch) -> `匯入函式庫` (Include Library) -> `加入 .ZIP 函式庫...` (Add .ZIP Library...)。
   * 選擇您剛才下載的 `.zip` 檔案進行匯入。
3. **手動安裝（替代方案）**：
   * 將解壓縮後的資料夾直接複製到您電腦中的 Arduino 專案路徑中的 `libraries` 資料夾下（通常位於 `C:\Users\<您的用戶名>\Documents\Arduino\libraries`）。
4. **相依性檢查**：
   * 本函式庫強烈依賴官方的 `USB_Host_Shield_2.0` 函式庫。請確保您已透過 Arduino 開發板管理員搜尋並安裝該基礎硬體支援庫。

---

## 3. API 參考指南

本函式庫的所有類別與方法皆封裝於 `damiao` 命名空間中，僅需引入一個標頭檔：
```cpp
#include <damiao_control.hpp>

####
 1. damiao::Motor_Vel
功能：速度模式下的馬達實體物件。

建構子：Motor_Vel(uint32_t slave_id, float max_rpm = 395.0f)

slave_id：馬達的 CAN 終端從機 ID。

max_rpm：該馬達允許的最大轉速限制（預設為 395.0 RPM）。

公開成員函式：

uint32_t GetSlaveId() const：獲取馬達的從機 ID。

float GetMaxRpm() const：獲取馬達目前設定的最大轉速。

#### 2. `damiao::Motor_PosVel`
* **功能**：位置速度模式下的馬達實體物件。
* **建構子**：`Motor_PosVel(uint32_t slave_id, float max_pos_rad = 1000.0f, float max_rpm = 395.0f)`
  * `slave_id`：馬達的 CAN 終端從機 ID。
  * `max_pos_rad`：最大位置限制（單位：弧度 rad，預設 1000.0f）。
  * `max_rpm`：最高速度限制（單位：RPM，預設 395.0f）。
* **公開成員函式**：
  * `uint32_t GetSlaveId() const`：獲取馬達的從機 ID。
  * `float GetMaxPosRad() const`：獲取目前設定的最大位置弧度。
  * `float GetMaxRpm() const`：獲取目前設定的最大轉速限制。

---

### 萬能控制器類別與方法 (Universal Controller)

#### `damiao::Motor_Control`
* **建構子**：`Motor_Control(USB* usb, ACM* acm)`
  * 傳入底層儲存的 USB 核心指標與 ACM 序列介面指標，統一管理全域硬體通訊狀態。
* **主要函式**：

| 函式名稱 | 參數說明 | 回傳值 | 功能簡述 |
| :--- | :--- | :--- | :--- |
| `void init()` | 無 | `void` | 初始化 USB Host 硬體，並自動設定 ACM 序列通訊鮑率為 921600 bps。全域僅需調用一次。 |
| `bool isReady()` | 無 | `bool` | 檢查控制器與底層 USB ACM 裝置是否已就緒。 |
| `void enable(const Motor_Vel& motor)` | `motor`: 速度模式馬達 | `void` | [函式多載] 發送使能指令（`0xFC`），啟用指定的轉速馬達。 |
| `void enable(const Motor_PosVel& motor)` | `motor`: 位置速度馬達 | `void` | [函式多載] 發送使能指令（`0xFC`），啟用指定的位置速度馬達。 |
| `void disable(const Motor_Vel& motor)` | `motor`: 速度模式馬達 | `void` | [函式多載] 發送失能指令（`0xFD`），解除指定速度馬達的受控狀態。 |
| `void disable(const Motor_PosVel& motor)` | `motor`: 位置速度馬達 | `void` | [函式多載] 發送失能指令（`0xFD`），解除指定位置速度馬達的受控狀態。 |
| `void control_vel(const Motor_Vel& motor, float vel_rpm)` | `motor`: 速度馬達物件<br>`vel_rpm`: 目標轉速 (RPM) | `void` | 驅動馬達以設定 RPM 運轉。內部會自動依據馬達的 `max_rpm` 進行安全限幅，並自動換算為达妙協定所需的弧度/秒 (rad/s) 訊號。 |
| `void control_pos_vel(const Motor_PosVel& motor, float output_pos_deg, float vel_rpm)` | `motor`: 位置速度馬達<br>`output_pos_deg`: 輸出軸目標角度 (度)<br>`vel_rpm`: 最高速度限制 (RPM) | `void` | 控制馬達移動到指定的輸出軸角度（單位：度）。內部已自動套用大廟馬達內部減速比（3591.0 / 187.0）計算出內部的實際控制弧度，並進行邊界安全限幅。 |

---

## 4. 使用範例：雙模式混合控制

以下範例展示如何宣告**單一個控制器實例**，同時平穩地對「速度模式馬達 (ID: 0x03)」與「位置速度模式馬達 (ID: 0x04)」進行多工即時重新整理控制：

```cpp
#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>
#include <damiao_control.hpp> // 引入統一的萬能控制標頭檔

// 1. 宣告底層 USB Host Shield 硬體實例
USB Usb;
class ACMAsyncOper : public CDCAsyncOper {
public:
    uint8_t OnInit(ACM *pacm) override { return 0; }
};
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

// 2. 實例化單一控制器（全域唯一，管理完整通訊狀態）
damiao::Motor_Control ctrl(&Usb, &Acm);

// 3. 定義不同控制需求的馬達物件
damiao::Motor_Vel    motor3_vel(0x03, 395.0f);              // 馬達 3 採用速度模式
damiao::Motor_PosVel motor4_posvel(0x04, 1000.0f, 395.0f);   // 馬達 4 採用位置速度模式

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    Serial.println("[Dual Mode Demo] Initializing USB Host...");
    // 統一由這個 ctrl 初始化底層硬體與通訊環境
    ctrl.init(); 
    
    Serial.println("Enabling Motor 3 (Velocity) and Motor 4 (Position-Velocity)...");
    ctrl.enable(motor3_vel);
    ctrl.enable(motor4_posvel); // 多載設計，直接呼叫同一個 ctrl 即可
    delay(200); // 留出時間給馬達建立使能狀態
}

void loop() {
    Serial.println("--- Step 1: Motor3 正轉 100 RPM, Motor4 移動到 45.0 度 ---");
    // 持續 2 秒 (100 * 20ms)，確保馬達能夠在控制週期內穩定運作
    for (int i = 0; i < 100; i++) {
        ctrl.control_vel(motor3_vel, 100.0f);
        ctrl.control_pos_vel(motor4_posvel, 45.0f, 50.0f); // 目標 45 度，最高速度限制 50 RPM
        delay(20); // 嚴格維持达妙馬達推薦的 20ms 刷新週期
    }
    
    Serial.println("--- Step 2: Motor3 反轉 -50 RPM, Motor4 回到 0.0 度 ---");
    for (int i = 0; i < 100; i++) {
        ctrl.control_vel(motor3_vel, -50.0f);
        ctrl.control_pos_vel(motor4_posvel, 0.0f, 50.0f);  // 目標 0 度，最高速度限制 50 RPM
        delay(20); // 20ms 控制週期
    }
}
