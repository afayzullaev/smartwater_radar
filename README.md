# SmartWater Radar

An ESP32-based Smart Water Radar System that helps monitor and manage water usage efficiently using RS485 communication, cellular connectivity, and various sensors.

## Overview

This project uses an ESP32 microcontroller to create a smart water monitoring system. It combines radar-based flow measurement with cellular connectivity (A7670 modem) to provide real-time water usage monitoring, leak detection, and data transmission capabilities. The system uses RS485 communication for the radar sensor and PPP protocol for internet connectivity.

## Features

- Water flow monitoring using RS485 radar sensor
- Cellular connectivity using A7670 modem with PPP protocol
- Multiple sensor support:
  - Radar sensor for flow measurement
  - TDS (Total Dissolved Solids) sensor
  - Pressure sensor
  - Ultrasonic sensor
  - ADC-based sensors
- Real-time data visualization
- Low power operation with power management
- SD card support for local data storage
- Modbus RTU protocol support over RS485
- HTTP capabilities for data transmission
- GPS/GNSS location support via A7670 modem

## Hardware Requirements

- ESP32 Development Board
- A7670 Cellular Modem
- RS485 Radar Flow Sensor (Modbus RTU, Address: 0x34)
- Additional sensors (optional):
  - TDS sensor
  - Pressure sensor
  - Ultrasonic sensor
- SD Card (for data logging)
- Power supply

## Technical Specifications

### Radar Sensor
- Communication: RS485 (Modbus RTU)
- Device Address: 0x34
- Register Start: 0x01F6
- Function Code: 0x03
- Baud Rate: 9600 bps

### Cellular Modem (A7670)
- UART Configuration:
  - TX Pin: GPIO26
  - RX Pin: GPIO27
  - Control Pins:
    - RESET: GPIO5
    - PWKEY: GPIO4
- Network: PPP Protocol
- APN: "internet.beeline.uz"
- Additional Features:
  - GNSS/GPS support
  - Signal quality monitoring
  - Power management

### RS485 Interface
- UART Port: UART2
- Baud Rate: 9600
- Pins:
  - TX: GPIO22
  - RX: GPIO23
  - RTS: GPIO21
  - Power Control: GPIO33

## Software Requirements

- ESP-IDF (Espressif IoT Development Framework)
- CMake
- Python 3.x (for ESP-IDF tools)

## Project Structure

```
├── CMakeLists.txt          # Project CMake configuration
├── main/                   # Main source code directory
│   ├── CMakeLists.txt     # Main component CMake configuration
│   ├── main.c             # Main application code
│   └── libs/              # Library modules
│       ├── inc/           # Header files
│       │   ├── radarsensor.h     # Radar sensor interface
│       │   ├── modemrelated.h    # A7670 modem functions
│       │   ├── modbussettings.h  # RS485/Modbus configuration
│       │   └── ...              # Other sensor interfaces
│       └── src/           # Implementation files
├── components/            # Custom components directory
└── README.md             # This file
```

## Building and Flashing

1. Set up ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

2. Build the project:
   ```bash
   idf.py build
   ```

3. Flash to your ESP32:
   ```bash
   idf.py -p (PORT) flash
   ```
   Replace (PORT) with your ESP32's serial port (e.g., /dev/ttyUSB0 on Linux or /dev/cu.SLAB_USBtoUART on macOS)

4. Monitor the output:
   ```bash
   idf.py monitor
   ```

## Communication Flow

1. The ESP32 initializes the RS485 interface for radar sensor communication
2. The A7670 modem is initialized and establishes a PPP connection
3. Radar sensor data is read periodically via Modbus RTU
4. Additional sensor data is collected as configured
5. Data is processed and can be:
   - Stored locally on SD card
   - Transmitted via cellular connection
   - Used for real-time monitoring and control

## Power Management

The system includes power management features for both the modem and sensors:
- Modem power control via PWKEY and RESET pins
- RS485 power control
- Low power modes for extended battery operation

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
