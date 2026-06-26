#ifndef __DAMIAO_VAL_HPP__
#define __DAMIAO_VAL_HPP__

#include <Arduino.h>
#include <Usb.h>
#include <cdcacm.h>

namespace damiao {

#ifndef __DAMIAO_CAN_FRAME__
#define __DAMIAO_CAN_FRAME__
#pragma pack(1)
struct can_send_frame {
    uint8_t FrameHeader[2] = {0x55, 0xAA}; 
    uint8_t FrameLen = 0x1e;               
    uint8_t CMD = 0x03;                    
    uint32_t sendTimes = 1;                
    uint32_t timeInterval = 10;            
    uint8_t IDType = 0;                    
    uint32_t canId = 0x01;                 
    uint8_t frameType = 0;                 
    uint8_t len = 0x08;                    
    uint8_t idAcc = 0;
    uint8_t dataAcc = 0;
    uint8_t data[8] = {0};
    uint8_t crc = 0;

    void modify(uint32_t id, const uint8_t* send_data) {
        canId = id;
        memcpy(data, send_data, 8);
    }
};
#pragma pack()
#endif

#define SPEED_MODE 0x200

class Motor_Vel {
private:
    uint32_t slave_id;
    float max_rpm;

public:
    Motor_Vel(uint32_t slave_id, float max_rpm = 395.0f) 
        : slave_id(slave_id), max_rpm(max_rpm) {}

    uint32_t GetSlaveId() const { return slave_id; }
    float GetMaxRpm() const { return max_rpm; }
};

class Motor_Control_Vel {
private:
    USB* usb_ptr;
    ACM* acm_ptr;
    bool is_device_ready;
    can_send_frame send_frame;

    void usb_serial_write(const uint8_t* buf, uint16_t len);
    void control_cmd(uint32_t id, uint8_t cmd);

public:
    Motor_Control_Vel(USB* usb, ACM* acm) 
        : usb_ptr(usb), acm_ptr(acm), is_device_ready(false) {}

    void init();
    bool isReady() { return is_device_ready; }
    void enable(const Motor_Vel& motor);
    void disable(const Motor_Vel& motor);
    void control_vel(const Motor_Vel& motor, float vel_rpm);
};

} // namespace damiao

#endif