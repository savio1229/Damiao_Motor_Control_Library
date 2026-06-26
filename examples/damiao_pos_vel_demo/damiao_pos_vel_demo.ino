#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>
#include <damiao_pos_vel.hpp> // 引入位置速度模式標頭檔

// 建立 USB Host Shield 必備硬體物件
USB Usb;
class ACMAsyncOper : public CDCAsyncOper {
public:
    uint8_t OnInit(ACM *pacm) override { return 0; }
};
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

// 實例化位置速度模式控制器
damiao::Motor_Control_PosVel motor_ctrl(&Usb, &Acm);

// 定義位置速度模式下的馬達物件 (ID 4)
damiao::Motor_PosVel motor4(0x04, 1000.0f, 395.0f);

void setup() {
    Serial.begin(115200);
    while(!Serial); 
    
    Serial.println("[Position-Velocity Mode] Initializing USB Host...");
    motor_ctrl.init();
    
    Serial.println("USB2CAN Ready! Enabling Motor 4...");
    motor_ctrl.enable(motor4);
    delay(200); 
}

void loop() {
    Serial.println("Moving Output Shaft to 90.0 degrees (Max 50 RPM)...");
    for (int i = 0; i < 100; i++) {
        motor_ctrl.control_pos_vel(motor4, 90.0f, 50.0f);
        delay(20); // 維持 20ms 控制週期
    }
    delay(2000);

    Serial.println("Moving Output Shaft back to 0.0 degrees (Max 100 RPM)...");
    for (int i = 0; i < 100; i++) {
        motor_ctrl.control_pos_vel(motor4, 0.0f, 100.0f);
        delay(20);
    }
    delay(2000); 
}