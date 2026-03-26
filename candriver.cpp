#include "candriver.hpp"

#include <cerrno>
#include <cstring>
#include <chrono>
#include <fstream>
#include <iostream>
#include <fcntl.h>  // added for non-blocking mode

// -------------------------
// Lifecycle / init
// -------------------------
CAN_Handler::~CAN_Handler() {
    if (s_ >= 0) {
        ::close(s_);
        s_ = -1;
    }
}

bool CAN_Handler::init(const char* ifname) {
    sockaddr_can addr{};
    ifreq ifr{};

    s_ = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s_ < 0) {
        std::perror("socket");
        return false;
    }

    std::strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';

    if (::ioctl(s_, SIOCGIFINDEX, &ifr) < 0) {
        std::perror("ioctl");
        ::close(s_);
        s_ = -1;
        return false;
    }

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (::bind(s_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("bind");
        ::close(s_);
        s_ = -1;
        return false;
    }

    // added to make socket non-blocking so read() doesn't freeze the dashboard
    int flags = fcntl(s_, F_GETFL, 0);
    if (flags == -1) {
        std::perror("fcntl get");
        ::close(s_);
        s_ = -1;
        return false;
    }
    
    if (fcntl(s_, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::perror("fcntl set nonblock");
        ::close(s_);
        s_ = -1;
        return false;
    }

    return true;
}

// Call this once per control tick before you read/compute/send.
// This guarantees "old values" are one tick behind.
void CAN_Handler::snapshot_tick() {
    // No prev/cur snapshotting in this CanBus layout.
}

// -------------------------
// RX: read + decode
// -------------------------
bool CAN_Handler::read_and_decode_once() {
    can_frame frame{};
    const int n = ::read(s_, &frame, sizeof(frame));
    
    // CHANGED: Don't treat "would block" as an error - it's expected!
    if (n != static_cast<int>(sizeof(frame))) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available right now - that's OK!
            return true;  // Return success, just no new data
        }
        // Real error
        std::perror("read");
        return false;
    }

    const uint32_t can_id = frame.can_id & CAN_EFF_MASK;

    decode_powertrain(can_id, frame.data, frame.can_dlc);
    decode_chassis(can_id, frame.data, frame.can_dlc);

    return true;
}

void CAN_Handler::decode_powertrain(uint32_t can_id,
                                    const uint8_t* data,
                                    uint8_t dlc) {
    switch (can_id) {
        case 0x3:
            pedals_unpack(&bus_.pedals, data, dlc);
            break;

        case 0x4:
            wheel_unpack(&bus_.wheel, data, dlc);
            {
                using namespace std::chrono;
                static std::ofstream file("dial_mode.csv", std::ios::app);
                static bool initialized = false;
                if (!initialized) {
                    if (file) {
                        if (file.tellp() == 0) {
                            file << "timestamp_ms,dial_mode\n";
                        }
                    }
                    initialized = true;
                }
                if (file) {
                    auto now = steady_clock::now();
                    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
                    std::cout << ms << "," << static_cast<int>(bus_.wheel.dial_mode) << std::endl;
                }
            }
            break;

// add all the other ones
// powertrain dbc

        case 0x283:
            amk_actval_1_fl_unpack(&bus_.amk_actval_1_fl, data, dlc);
            break;
        
        case 0x285:
            amk_actval_2_fl_unpack(&bus_.amk_actval_2_fl, data, dlc);
            break;
        
        case 0x284:
            amk_actval_1_fr_unpack(&bus_.amk_actval_1_fr, data, dlc);
            break;

        case 0x286:
            amk_actval_2_fr_unpack(&bus_.amk_actval_2_fr, data, dlc);
            break;
        
        case 0x287:
            amk_actval_1_rl_unpack(&bus_.amk_actval_1_rl, data, dlc);
            break;
        
        case 0x289:
            amk_actval_2_rl_unpack(&bus_.amk_actval_2_rl, data, dlc);
            break;

        case 0x288:
            amk_actval_1_rr_unpack(&bus_.amk_actval_1_rr, data, dlc);
            break;

        case 0x290:
            amk_actval_2_rr_unpack(&bus_.amk_actval_2_rr, data, dlc);
            break;

        case 0x184:
            amk_setpt_fl_unpack(&bus_.amk_setpt_fl, data, dlc);
            break;

        case 0x90:
            ams_status_unpack(&bus_.ams_status, data, dlc);
            break;

        /*case 0x285:
            ams_status_pack_state_unpack(&bus_.ams_status_pack_state, data, dlc);
            break;*/

        case 0x91:
            ams_volt_summary_unpack(&bus_.ams_volt_summary, data, dlc);
            break;

        case 0x92:
            ams_tab_temp_summary_unpack(&bus_.ams_tab_temp_summary, data, dlc);
            break;
        
        case 0x93:
            ams_cell_temp_summary_unpack(&bus_.ams_cell_temp_summary, data, dlc);
            break;
        
        case 0x94:
            ams_faults_unpack(&bus_.ams_faults, data, dlc);
            break;

        case 0x95:
            ams_mask_unpack(&bus_.ams_mask, data, dlc);
            break;

        case 0x96:
            ams_energetics_unpack(&bus_.ams_energetics, data, dlc);
            break;
        
        default:
            break;
    }
}

void CAN_Handler::decode_chassis(uint32_t can_id,
                                 const uint8_t* data,
                                 uint8_t dlc) {
    // Add chassis IDs later
    switch (can_id) {
    //chassis dbc
        case 0x10:
            rear_1_unpack(&bus_.rear1, data, dlc);
            break;

        case 0x11:
            rear_2_unpack(&bus_.rear2, data, dlc);
            break;

        case 0x14:
            rear_3_unpack(&bus_.rear3, data, dlc);
            break;

        case 0x18:
            lv_pwr_override_unpack(&bus_.lv_pwr_override, data, dlc);
            break;

        case 0x19:
            lv_pwr_ctrl_unpack(&bus_.lv_pwr_ctrl, data, dlc);
            break;

        case 0x20:
            front_1_unpack(&bus_.front1, data, dlc);
            break;

        case 0x21:
            front_2_unpack(&bus_.front2, data, dlc);
            break;

        case 0x22:
            front_3_unpack(&bus_.front3, data, dlc);
            break;
        
        case 0x118:
            state_machine_unpack(&bus_.statemachine, data, dlc);
            break;

        case 0x121:
            speed_estimation_unpack(&bus_.speed, data, dlc);
            break;
            
        case 0x136:
            driver_inputs_unpack(&bus_.driverinputs, data, dlc);
            break;
        
        default:
            break;
    }
} 

// -------------------------
// TX: encode + send
// -------------------------

// Packs a CAN frame payload from bus_.cur_tx based on can_id.
// Returns true if this can_id is supported.
bool CAN_Handler::encode_signals(uint32_t can_id, uint8_t* data, uint8_t& dlc) const {
    switch (can_id) {
        case 0x184: {
            dlc = 8;
            amk_setpt_fl_pack(data, &bus_.amk_setpt_fl, dlc);
            return true;
        }

        default:
            dlc = 0;
            return false;
    }
}

// Low-level write of a raw CAN frame.
bool CAN_Handler::send_frame(uint32_t can_id, const uint8_t* data, uint8_t dlc) {
    can_frame frame{};
    frame.can_id  = can_id;
    frame.can_dlc = dlc;
    std::memcpy(frame.data, data, dlc);

    const int n = ::write(s_, &frame, sizeof(frame));
    if (n != static_cast<int>(sizeof(frame))) {
        std::perror("write");
        return false;
    }
    return true;
}

// Convenience: encode+send one message by id.
bool CAN_Handler::send_messages() {
    if (s_ < 0) return false;

    bool ok = true;

    // List the CAN IDs you want to transmit each tick
    static constexpr uint32_t kTxIds[] = {
        0x184,  // AMK_Setpt_FL
        // 0x185,  // AMK_Setpt_FR
        // 0x186,  // AMK_Setpt_RL
        // 0x187,  // AMK_Setpt_RR
    };

    for (uint32_t id : kTxIds) {
        uint8_t data[8] = {0};
        uint8_t dlc = 0;

        // Pack payload from bus_.cur_tx
        if (!encode_signals(id, data, dlc) || dlc == 0) {
            ok = false;
            continue;
        }

        // Write to socket
        ok &= send_frame(id, data, dlc);
    }

    return ok;
}