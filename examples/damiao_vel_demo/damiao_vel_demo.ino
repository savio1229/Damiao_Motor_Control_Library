#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>
#include <damiao_val.hpp> // 引入速度模式標頭檔

// 建立 USB Host Shield 必備硬體物件
USB Usb;
class ACMAsyncOper : public CDCAsyncOper {
public:
    uint8_t OnInit(ACM *pacm) override { return 0; }
};
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

// 實例化速度模式控制器
damiao::Motor_Control_Vel motor_ctrl(&Usb, &Acm);

// 定義速度模式下的馬達物件 (ID 3, ID 4)
damiao::Motor_Vel motor3(0x03, 395.0f); 
damiao::Motor_Vel motor4(0x04, 395.0f);

void setup() {
    Serial.begin(115200);
    while(!Serial); 
    
    Serial.println("[Velocity Mode] Initializing USB Host...");
    motor_ctrl.init();
    
    Serial.println("USB2CAN Device Ready! Enabling Motors...");
    motor_ctrl.enable(motor3);
    motor_ctrl.enable(motor4);
    delay(200);
}

void loop() {
    Serial.println("Step 1: Motor3 100 RPM, Motor4 -50 RPM");
    motor_ctrl.control_vel(motor3, 100.0f);
    motor_ctrl.control_vel(motor4, -50.0f);
    delay(1000); 
    
    Serial.println("Step 2: Motor3 -150 RPM, Motor4 100 RPM");
    motor_ctrl.control_vel(motor3, -150.0f);
    motor_ctrl.control_vel(motor4, 100.0f);
    delay(1000); 
}