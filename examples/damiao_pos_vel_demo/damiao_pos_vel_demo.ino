#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>
#include <damiao_control.hpp> // 引入統一的控制標頭檔

// 1. 底層 USB Host Shield 硬體實例
USB Usb;
class ACMAsyncOper : public CDCAsyncOper {
public:
    uint8_t OnInit(ACM *pacm) override { return 0; }
};
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

// 2. 實例化統一的控制器
damiao::Motor_Control ctrl(&Usb, &Acm);

// 3. 定義一顆位置速度模式的馬達
// 參數：(CAN ID, 最大限制弧度, 最大限制 RPM)
damiao::Motor_PosVel motor4_posvel(0x04, 1000.0f, 395.0f); 

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    Serial.println("[Pure Position-Velocity Demo] Initializing USB Host...");
    ctrl.init(); // 初始化硬體與設定鮑率
    
    Serial.println("Enabling Motor 4 (Position-Velocity Mode)...");
    ctrl.enable(motor4_posvel); // 使能馬達
    delay(200); // 留出時間讓馬達建立使能狀態
}

void loop() {
    Serial.println(">>> Action: Move to 90.0 Degrees (Slowly: Max 30 RPM)");
    // 持續 3 秒 (150 次 * 20ms)，給馬達足夠的時間走到目標角度
    for(int i = 0; i < 150; i++) {
        // 參數：(馬達物件, 目標角度度數, 限制最高轉速)
        ctrl.control_pos_vel(motor4_posvel, 90.0f, 30.0f); 
        delay(20); 
    }
    
    delay(500); // 在 90 度稍微停頓 0.5 秒
    
    Serial.println(">>> Action: Return to 0.0 Degrees (Fast: Max 60 RPM)");
    for(int i = 0; i < 150; i++) {
        ctrl.control_pos_vel(motor4_posvel, 0.0f, 60.0f); 
        delay(20);
    }

    delay(500); // 在 0 度稍微停頓 0.5 秒
}