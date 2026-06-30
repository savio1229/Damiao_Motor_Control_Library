#include "damiao_control.hpp"

namespace damiao {

void Motor_Control::init() {
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

void Motor_Control::usb_serial_write(const uint8_t* buf, uint16_t len) {
    if (!is_device_ready) return;
    acm_ptr->SndData(len, const_cast<uint8_t*>(buf));
}

void Motor_Control::control_cmd(uint32_t id, uint8_t cmd) {
    usb_ptr->Task();
    if (!is_device_ready) return;
    uint8_t data_buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, cmd};
    send_frame.modify(id, data_buf);
    usb_serial_write(reinterpret_cast<uint8_t*>(&send_frame), sizeof(can_send_frame));
}

// 實作多載使能控制
void Motor_Control::enable(const Motor_Vel& motor) {
    control_cmd(motor.GetSlaveId(), 0xFC);
}
void Motor_Control::disable(const Motor_Vel& motor) {
    control_cmd(motor.GetSlaveId(), 0xFD);
}
void Motor_Control::enable(const Motor_PosVel& motor) {
    control_cmd(motor.GetSlaveId(), 0xFC);
}
void Motor_Control::disable(const Motor_PosVel& motor) {
    control_cmd(motor.GetSlaveId(), 0xFD);
}

// 速度控制實作
void Motor_Control::control_vel(const Motor_Vel& motor, float vel_rpm) {
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

// 位置速度控制實作
void Motor_Control::control_pos_vel(const Motor_PosVel& motor, float output_pos_deg, float vel_rpm) {
    usb_ptr->Task();
    if (!is_device_ready) return;

    const float GEAR_RATIO = 3591.0f / 187.0f;
    const float LOCAL_DEG_TO_RAD = 0.0174532925f;

    float motor_pos_rad = output_pos_deg * LOCAL_DEG_TO_RAD * GEAR_RATIO;

    if (motor_pos_rad >  motor.GetMaxPosRad()) motor_pos_rad =  motor.GetMaxPosRad();
    if (motor_pos_rad < -motor.GetMaxPosRad()) motor_pos_rad = -motor.GetMaxPosRad();

    if (vel_rpm >  motor.GetMaxRpm()) vel_rpm =  motor.GetMaxRpm();
    if (vel_rpm < -motor.GetMaxRpm()) vel_rpm = -motor.GetMaxRpm();

    float vel_rad_s = vel_rpm * 0.1047197551f;

    uint8_t data_buf[8] = {0};
    memcpy(&data_buf[0], &motor_pos_rad, sizeof(float));
    memcpy(&data_buf[4], &vel_rad_s, sizeof(float));

    uint32_t target_can_id = POS_VEL_MODE + motor.GetSlaveId();
    send_frame.modify(target_can_id, data_buf);

    usb_serial_write(reinterpret_cast<uint8_t*>(&send_frame), sizeof(can_send_frame));
}

} // namespace damiao