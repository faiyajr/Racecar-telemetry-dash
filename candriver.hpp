#pragma once

#include <cstdint>
#include <cstring>
#include <cstdio>

#include <unistd.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include "CanBus.hpp"  

class CAN_Handler {
public:
    CAN_Handler() = default;
    ~CAN_Handler();

    CAN_Handler(const CAN_Handler&) = delete;
    CAN_Handler& operator=(const CAN_Handler&) = delete;

    bool init(const char* ifname = "can0");
    bool read_and_decode_once();

    void snapshot_tick();
    bool send_messages();

    const CanBus& bus() const { return bus_; }
    CanBus& bus_mut() { return bus_; }

private:
    void decode_powertrain(uint32_t can_id, const uint8_t* data, uint8_t dlc);
    void decode_chassis(uint32_t can_id, const uint8_t* data, uint8_t dlc);

    bool encode_signals(uint32_t can_id, uint8_t* data, uint8_t& dlc) const;
    bool send_frame(uint32_t can_id, const uint8_t* data, uint8_t dlc);

    int s_ = -1;
    CanBus bus_{};
};
