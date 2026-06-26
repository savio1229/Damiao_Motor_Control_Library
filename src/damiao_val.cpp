#include "damiao_val.hpp"

namespace damiao {

void Motor_Control_Vel::init() {
    if (usb_ptr->Init() == -1) {
        while (1) { 
            Serial.println("USB Host Init Failed!"); 
            delay(500); 
        } 
    }
    
    while (!acm_ptr->isReady()) {
        usb_ptr->Task();
    }
    is_device_ready = true;

    LINE_CODING lc;
    lc.dwDTERate   = 921600;
    lc.bCharFormat = 0;
    lc.bParityType = 0;
    lc.bDataBits   = 8;
    acm_ptr->SetLineCoding(&lc);
    delay(100);
}

void Motor_Control_Vel::usb_serial_write(const uint8_t* buf, uint16_t len) {
    if (!is_device_ready) return;
    acm_ptr->SndData(len, const_cast<uint8_t*>(buf));
}

void Motor_Control_Vel::control_cmd(uint32_t id, uint8_t cmd) {
    usb_ptr->Task();
    if (!is_device_ready) return;

    uint8_t data_buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, cmd};
    send_frame.modify(id, data_buf);
    usb_serial_write(reinterpret_cast<uint8_t*>(&send_frame), sizeof(can_send_frame));
}

void Motor_Control_Vel::enable(const Motor_Vel& motor) {
    control_cmd(motor.GetSlaveId(), 0xFC);
}

void Motor_Control_Vel::disable(const Motor_Vel& motor) {
    control_cmd(motor.GetSlaveId(), 0xFD);
}

void Motor_Control_Vel::control_vel(const Motor_Vel& motor, float vel_rpm) {
    usb_ptr->Task();
    if (!is_device_ready) return;

    if (vel_rpm >  motor.GetMaxRpm()) vel_rpm =  motor.GetMaxRpm();
    if (vel_rpm < -motor.GetMaxRpm()) vel_rpm = -motor.GetMaxRpm();

    float rad_s = vel_rpm * 0.1047197551f;

    union { float f; uint32_t u; } v;
    v.f = rad_s;

    uint8_t data_buf[8] = {0};
    data_buf[0] = (uint8_t)(v.u & 0xFF);
    data_buf[1] = (uint8_t)((v.u >> 8) & 0xFF);
    data_buf[2] = (uint8_t)((v.u >> 16) & 0xFF);
    data_buf[3] = (uint8_t)((v.u >> 24) & 0xFF);

    uint32_t target_can_id = SPEED_MODE + motor.GetSlaveId();
    send_frame.modify(target_can_id, data_buf);

    usb_serial_write(reinterpret_cast<uint8_t*>(&send_frame), sizeof(can_send_frame));
}

} // namespace damiao