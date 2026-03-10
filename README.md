# Racing Dashboard for Raspberry Pi

A real-time racing dashboard display system for electric formula racing vehicles, built with SDL2 and designed to run on Raspberry Pi with a 800x480 display.

## Overview

This dashboard displays critical vehicle telemetry data from the CAN bus including:
- Vehicle speed
- Battery pack status and voltage
- Motor controller data
- Temperature monitoring
- Shutdown circuit status
- And more...

## Hardware Requirements

- Raspberry Pi (tested on Pi 4/5)
- 5-inch 800x480 display (or compatible)
- CAN interface (MCP2515 or similar)
- Font files (DejaVu Sans Bold)

## Software Dependencies

### Required Libraries
```bash
sudo apt update
sudo apt install -y libsdl2-dev libsdl2-ttf-dev liblgpio-dev
```

### CAN Tools (optional, for testing)
```bash
sudo apt install can-utils
```

## Project Structure

```
.
├── main.cpp                 # Main application entry point
├── DisplayManager.cpp/h     # SDL display management
├── DashboardUI.cpp/h        # UI layout and rendering
├── candriver.cpp/hpp        # CAN bus interface
├── dash.cpp/hpp             # GPIO control (LEDs)
├── CanBus.hpp              # CAN message data structures
├── powertrain.h            # Powertrain DBC definitions
├── chassis.h               # Chassis DBC definitions
└── assets/
    └── fonts/
        └── DejaVuSans-Bold.ttf
```

## Building

```bash
# Compile
g++ -o dashboard main.cpp DisplayManager.cpp DashboardUI.cpp \
    candriver.cpp dash.cpp \
    -lSDL2 -lSDL2_ttf -llgpio -std=c++17

# Or use your Makefile if you have one
make
```

## Configuration

### CAN Interface Setup

Before running, ensure your CAN interface is configured:

```bash
# Bring up CAN interface
sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up

# Verify CAN is up
ip link show can0
```

### Display Rotation

If your display is rotated incorrectly, add to `/boot/config.txt`:

```bash
sudo nano /boot/config.txt
```

Add one of these lines:
```
display_lcd_rotate=0    # No rotation (portrait)
display_lcd_rotate=1    # 90 degrees clockwise
display_lcd_rotate=2    # 180 degrees
display_lcd_rotate=3    # 270 degrees clockwise
```

Then reboot:
```bash
sudo reboot
```

### Font Installation

Ensure the font file exists at:
```
../assets/fonts/DejaVuSans-Bold.ttf
```

Or install system-wide:
```bash
sudo apt install fonts-dejavu
```

## Running

### Basic Usage

```bash
./dashboard
```

### Auto-start on Boot

Create a systemd service:

```bash
sudo nano /etc/systemd/system/dashboard.service
```

Add:
```ini
[Unit]
Description=Racing Dashboard
After=network.target

[Service]
Type=simple
User=pi
WorkingDirectory=/home/pi/dashboard
ExecStartPre=/sbin/ip link set can0 type can bitrate 500000
ExecStartPre=/sbin/ip link set can0 up
ExecStart=/home/pi/dashboard/dashboard
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

Enable and start:
```bash
sudo systemctl enable dashboard.service
sudo systemctl start dashboard.service
```

## Controls

- **ESC** or **Q**: Exit dashboard
- **SPACE**: Change pages (when implemented)
- **Ctrl+C**: Exit (terminal)
- **Steering Wheel Button**: Page change (via CAN)

## Dashboard Layout

### Page 1 (Main Display)

```
┌─────────────┬────────────┬──────────────┐
│   SPEED     │    Time    │   Pack V     │
│    (BIG)    │            │              │
│             ├────────────┼──────────────┤
│             │ Brake Bias │ Mean Cell V  │
├─────────────┼────────────┼──────────────┤
│ Pack State  │   LV Bat   │Mean Cell Temp│
├─────────────┼────────────┼──────────────┤
│ Car State   │   (empty)  │  SDC Stat    │
└─────────────┴────────────┴──────────────┘
```

## CAN Bus Configuration

The system expects these CAN IDs (from DBC files):

### Powertrain Bus
- `0x003`: Pedals
- `0x004`: Steering Wheel
- `0x090-096`: AMS (Battery Management)
- `0x184-189`: AMK Motor Controllers (Setpoints)
- `0x283-290`: AMK Motor Controllers (Actual Values)

### Chassis Bus
- `0x010-011`: Rear Sensors
- `0x014`: Current Monitoring
- `0x018-019`: LV Power Control
- `0x020-022`: Front Sensors

## Troubleshooting

### Display Not Showing

1. **Check if SDL initializes:**
   - Program should output error messages to console
   - Font loading errors indicate missing font files

2. **Verify display connection:**
   ```bash
   # Check for displays
   DISPLAY=:0 xrandr
   ```

3. **Test with simple SDL program:**
   - Compile and run `sdl_test.cpp` to verify SDL works

### CAN Not Working

1. **Check CAN interface:**
   ```bash
   ip link show can0
   candump can0
   ```

2. **Verify bitrate:**
   - Must match vehicle CAN bus (typically 500kbps)

3. **Check wiring:**
   - CAN-H and CAN-L properly connected
   - 120Ω termination resistors installed

### Display Rotated Wrong

- Edit `/boot/config.txt` and change `display_lcd_rotate` value
- Try values 0, 1, 2, or 3 until correct

### Fonts Not Loading

1. **Check font path:**
   ```bash
   ls -la ../assets/fonts/DejaVuSans-Bold.ttf
   ```

2. **Update path in code** if fonts are elsewhere

3. **Install system fonts:**
   ```bash
   sudo apt install fonts-dejavu
   ```

## Performance Notes

- **Real-time system**: No exceptions used (embedded system requirement)
- **Frame rate**: 60 FPS
- **CAN read**: Non-blocking single frame read per loop
- **Display**: Hardware accelerated SDL rendering

## Known Issues

- CAN interface must be up before starting (will exit if not available)
- Font loading failure will cause crash (exception thrown)
- Touch input not yet calibrated for rotated displays

## Future Improvements

- [ ] Page 2 implementation (detailed motor/error data)
- [ ] Touch input support
- [ ] Data logging to SD card
- [ ] Configuration file for customization
- [ ] Remove exception throws for true embedded compatibility

## License

[Your License Here]

## Contributors

[Your Name/Team]

## Support

For issues or questions, contact [your contact info]

---

**Note**: This is a real-time embedded system designed for racing applications. Ensure all safety-critical systems have proper redundancy and failsafes.
