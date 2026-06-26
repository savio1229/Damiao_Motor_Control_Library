#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>
#include <damiao_val.hpp>       // 引入速度控制
#include <damiao_pos_vel.hpp>   // 引入位置速度控制

// 1. 共用一組底層 USB Host Shield 硬體實例
USB Usb;
class ACMAsyncOper : public CDCAsyncOper {
public:
    uint8_t OnInit(ACM *pacm) override { return 0; }
};
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

// 2. 分別實例化兩種類型的控制器，均指向同一個硬體指標
damiao::Motor_Control_Vel    ctrl_vel(&Usb, &Acm);
damiao::Motor_Control_PosVel ctrl_posvel(&Usb, &Acm);

// 3. 定義不同控制需求的馬達
damiao::Motor_Vel    motor3_vel(0x03, 395.0f);    // 馬達 3 採用速度模式
damiao::Motor_PosVel motor4_posvel(0x04, 1000.0f, 395.0f); // 馬達 4 採用位置速度模式

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    Serial.println("[Dual Mode Demo] Initializing USB Host via ctrl_vel...");
    // 兩者共用硬體，只需對其中一個控制器調用 init() 初始化硬體與設定鮑率即可
    ctrl_vel.init(); 
    
    Serial.println("Enabling Motor 3 (Velocity) and Motor 4 (Position-Velocity)...");
    ctrl_vel.enable(motor3_vel);
    ctrl_posvel.enable(motor4_posvel);
    delay(200);
}

void loop() {
    Serial.println("--- Action Cycle Start ---");
    
    // 在 2 秒的時間內，持續刷新兩顆馬達的控制訊號
    // 馬達 3 維持 80 RPM 運轉；馬達 4 緩慢移動到 45.0 度位置
    for(int i = 0; i < 100; i++) {
        ctrl_vel.control_vel(motor3_vel, 80.0f);
        ctrl_posvel.control_pos_vel(motor4_posvel, 45.0f, 30.0f);
        delay(20); // 20ms 控制週期
    }
    
    // 改變目標：馬達 3 反轉 -80 RPM；馬達 4 回到 0 度位置
    for(int i = 0; i < 100; i++) {
        ctrl_vel.control_vel(motor3_vel, -80.0f);
        ctrl_posvel.control_pos_vel(motor4_posvel, 0.0f, 60.0f);
        delay(20);
    }
}