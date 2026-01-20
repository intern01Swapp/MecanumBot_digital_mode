# MecanumBot_digital_mode
Wireless Mecanum robot using RMCS-2303  motor drivers, Arduino Mega 2560, and ESP32. Features omni-directional motion (forward, lateral, diagonal, rotation) with closed-loop speed control using encoders and real-time Wi-Fi command input.
MecanumBot_digital_mode


## Project Structure
```
MecanumBot_digital_mode/
├── Arduino_Mega/        # Arduino Mega firmware (motor control & kinematics)
├── ESP32_recevr/        # ESP32 Wi-Fi receiver and command interface
├── .gitignore
└── README.md
```

## Hardware Used

- Controller: Arduino Mega 2560
- Wireless Module: ESP32 (Wi-Fi based command receiver)
- Motor Driver: RMCS-2303 Digital Servo Motor Driver
- Motors:
- Pro-Range Planetary Gear DC Motors (PG36M555-19.2K)
  - Rated Voltage: 12 V DC
  - Rated Speed: ~262 RPM
  - Gear Ratio: 19.2 : 1
- Encoders:
  - OE-37 Hall-Effect Magnetic Encoder
  - 7 PPR (per channel)
  - Quadrature output (A & B channels)
- Chassis:
  - 4-wheel mecanum drive chassis with omni-directional motion
- Power Supply:
  - External DC power supply (as per motor and driver ratings)
  
## Key Features

Omni-directional mecanum drive control

Digital mode operation of RMCS-2303 drivers

Closed-loop speed control using encoder feedback

Real-time wireless command input via ESP32

Modular firmware structure (Arduino + ESP32)

RMCS-2303 Motor Driver Library

This project uses the RMCS-2303 driver library for motor control in digital mode.

📄 Datasheet / Reference:
RMCS-2303 Digital Servo Motor Driver – Updated Datasheet
(Refer to the official RMCS-2303 documentation provided by the manufacturer)

⚠️ Note: Ensure the RMCS-2303 driver is configured in digital mode and baud rate settings match the firmware.

Communication Flow
Wi-Fi Commands → ESP32 → Serial/UART → Arduino Mega → RMCS-2303 → BLDC Motors

Notes

venv, build artifacts, and temporary files are excluded via .gitignore

This repository focuses on firmware and control logic

Mechanical design and PCB files are not included
