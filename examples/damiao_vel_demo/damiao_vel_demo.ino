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

// 3. 定義一顆純速度控制模式的馬達
// 參數：(CAN ID, 最大限制 RPM)
damiao::Motor_Vel motor3_vel(0x03, 395.0f); 

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    Serial.println("[Pure Velocity Demo] Initializing USB Host...");
    ctrl.init(); // 初始化硬體與設定鮑率
    
    Serial.println("Enabling Motor 3 (Velocity Mode)...");
    ctrl.enable(motor3_vel); // 使能馬達
    delay(200); // 留出時間讓馬達建立使能狀態
}

void loop() {
    Serial.println(">>> Action: Forward 120 RPM");
    // 持續 2 秒 (100 次 * 20ms)，維持大廟馬達推薦的 20ms 刷新週期
    for(int i = 0; i < 100; i++) {
        ctrl.control_vel(motor3_vel, 120.0f); // 目標速度 120 RPM
        delay(20); 
    }
    
    Serial.println(">>> Action: Reverse -120 RPM");
    for(int i = 0; i < 100; i++) {
        ctrl.control_vel(motor3_vel, -120.0f); // 目標速度 -120 RPM
        delay(20);
    }
}