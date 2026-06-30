#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>
#include <damiao_control.hpp> // 引入統一的控制標頭檔

// 1. 底層硬體
USB Usb;
class ACMAsyncOper : public CDCAsyncOper {
public:
    uint8_t OnInit(ACM *pacm) override { return 0; }
};
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

// 2. 只需宣告一個統一的控制器，完美的管理狀態
damiao::Motor_Control ctrl(&Usb, &Acm);

// 3. 宣告不同類型的馬達
damiao::Motor_Vel    motor3_vel(0x03, 395.0f);    
damiao::Motor_PosVel motor4_posvel(0x04, 1000.0f, 395.0f);

void setup() {
    Serial.begin(115200);
    while(!Serial);
    
    Serial.println("[Dual Mode Demo] Initializing USB Host...");
    ctrl.init(); // 完美的單一初始化
    
    Serial.println("Enabling Motor 3 (Velocity) and Motor 4 (Position-Velocity)...");
    ctrl.enable(motor3_vel);
    ctrl.enable(motor4_posvel); // 透過多載，調用同一個 ctrl
    delay(200);
}

void loop() {
    Serial.println("--- Action Cycle Start ---");
    
    for(int i = 0; i < 100; i++) {
        ctrl.control_vel(motor3_vel, 80.0f);
        ctrl.control_pos_vel(motor4_posvel, 90.0f, 30.0f);
        delay(20); 
    }
    
    for(int i = 0; i < 100; i++) {
        ctrl.control_vel(motor3_vel, -80.0f);
        ctrl.control_pos_vel(motor4_posvel, 0.0f, 60.0f);
        delay(20);
    }
}