# MR26 Racing Dashboard

Real-time dashboard display for an electric formula racing vehicle, running on a Raspberry Pi with an 800×480 touchscreen. Reads live CAN bus data and renders a graphical UI via SDL2.

---

## Quick Start

### Build and Run
```bash
cd mr26-firmware/mr26-dash/build
cmake ..
cmake --build .
./vcu
```

### Controls
| Input | Action |
|---|---|
| `ESC` / `Q` | Exit dashboard |
| `SPACE` | Switch pages |
| Steering wheel button | Switch pages (via CAN) |

---

## Hardware Setup

### CAN Interface
```bash
sudo ip link set can0 down
sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up
```

---

## Dependencies

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev liblgpio-dev can-utils
```

---

## File Structure

```
mr26-dash/core/src/
├── main.cpp              # Main loop, SDL event handling, page switching
├── DashboardUI.cpp       # UI layout and rendering (Page 1 & 2)
├── DashboardUI.h
├── DisplayManager.cpp    # SDL2 draw primitives (text, rects, bars, popups)
├── DisplayManager.h
├── candriver.cpp         # CAN socket init, RX decode, TX encode/send
├── candriver.hpp
├── CanBus.hpp            # Aggregated CAN signal struct (powertrain + chassis)
├── LED.cpp               # GPIO LED control via lgpio
├── LED.hpp
└── assets/
    └── fonts/            # TTF font files
```

---

## Dashboard Pages

### Page 1 — Main Display
- **Speed** (large center display)
- **Pack voltage**, cell voltage, cell temperature
- **SDC status** — green = closed, red = open
- **Pack state** and **car state**
- **LV battery**, brake bias, and current time

### Page 2 — Detailed Diagnostics
- **Cell voltage** — min and mean
- **Temperature** — cell and tab mean/max
- **AMS fault flags** — OV / UV / OT / OC
- **Motor temperatures** — FL / FR / RL / RR
- **Inverter temperatures** — all four corners
- **SOC** with filled progress bar
- **SDC open segments** — detailed breakdown

---

## Error Display

### Color Coding
| Color | Meaning |
|---|---|
| Gray | No CAN data received |
| Green | Normal / circuit closed |
| Red | Error / circuit open |
| Flashing red | Active fault |

### Critical Error Popups
Triggered by:
- **OVERVOLTAGE** — pack overvoltage fault
- **OVERTEMP** — pack overtemperature fault
- **SDC OPEN** — shutdown circuit open

Popup behavior:
- Flashes on/off every 600 ms (full disappear)
- Pulses between bright and dark red every 300 ms when visible
- Rendered at 400×150 px for driver visibility

---

## Data Validation

The dashboard only shows errors when pack voltage > 0.5 V, confirming valid CAN data is present. On startup or with no CAN data, all boxes remain gray and no popups or flashing occur.

---

## CAN Bus

The CAN socket is opened in **non-blocking mode** so reads never freeze the render loop. Each frame drains available messages before drawing.

### Powertrain Bus (500 kbps)

| CAN ID | Signal |
|---|---|
| `0x003` | Pedals |
| `0x004` | Wheel (dial mode, page button) |
| `0x283–0x290` | AMK motor actual values (FL/FR/RL/RR) |
| `0x184` | AMK setpoint FL (TX) |
| `0x090–0x096` | AMS status, voltages, temps, faults, energetics |

### Chassis Bus (500 kbps)

| CAN ID | Signal |
|---|---|
| `0x010–0x014` | Rear 1/2/3 |
| `0x018–0x019` | LV power override / control |
| `0x020–0x022` | Front 1/2/3 |
| `0x118` | State machine |
| `0x121` | Speed estimation |
| `0x136` | Driver inputs |

---

## LED Indicators

Controlled via GPIO using `lgpio`. Three indicator LEDs are driven from the Raspberry Pi:

| Signal | GPIO Pin |
|---|---|
| LV | 27 |
| IMD | 22 |
| BMS | 17 |

IMD and BMS are active-low (logic inverted). A `testSequence()` method cycles all three LEDs on startup for hardware verification.
