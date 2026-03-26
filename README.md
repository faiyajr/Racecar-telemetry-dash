# MR26 Racing Dashboard

Real-time dashboard display for electric formula racing vehicle on Raspberry Pi with 800x480 touchscreen.

## Quick Start

### Build and Run
```bash
cd mr26-firmware/mr26-dash/build
cmake ..
cmake --build .
./vcu
```

### Controls
- **ESC** or **Q** - Exit dashboard
- **SPACE** - Switch pages
- **Steering wheel button** - Switch pages (via CAN)

## Hardware Setup

### CAN Interface
```bash
sudo ip link set can0 down
sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up
```

## Dashboard Pages

### Page 1 - Main Display
- **Speed** (large display)
- **Pack voltage, cell voltage, cell temp**
- **SDC status** (green = closed, red = open)
- **Pack state, car state**
- **LV battery, brake bias, time**

### Page 2 - Detailed Diagnostics
- **Cell voltage** (min/mean)
- **Temperature** (cell & tab mean/max)
- **AMS errors** (OV/UV/OT/OC)
- **Motor temps** (FL/FR/RL/RR)
- **Inverter temps** (all 4 motors)
- **SOC** with progress bar
- **SDC open segments** (detailed breakdown)

## Error Display

### Color Coding
- **Gray boxes** - No CAN data
- **Green boxes** - Normal/closed
- **Red boxes** - Error/open
- **Flashing red** - Active errors

### Critical Error Popup
Displays for:
- **OVERVOLTAGE** - Pack overvoltage fault
- **OVERTEMP** - Pack overtemperature fault  
- **SDC OPEN** - Shutdown circuit open

**Behavior:** 
- Popup flashes on/off completely (300ms)
- Pulses bright/dark red when visible (150ms)
- Larger size (400x150px) for driver visibility

## Data Validation

Dashboard only shows errors when:
- Pack voltage > 0.5V (confirms valid CAN data)
- Prevents false errors on startup

Without CAN data:
- All boxes remain gray
- No error popups
- No flashing

## CAN Bus Configuration Canoe

**Powertrain Bus (500kbps):**
- based on inputs from dashboard.cpp signals

**Chassis Bus (500kbps):**
- based on inputs from dashboard.cpp signals

## Dependencies

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev liblgpio-dev can-utils
```

## File Structure

```
mr26-dash/core/src
├── main.cpp              # Main loop
├── DashboardUI.cpp       # UI layout and rendering
├── DisplayManager.cpp    # SDL display functions
├── candriver.cpp         # CAN bus interface
├── LED.cpp              # GPIO LED control
└── assets/
    └── fonts/           # Font files
```
