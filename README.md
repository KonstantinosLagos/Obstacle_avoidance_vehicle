# Obstacle Avoidance UGV (Unmanned Ground Vehicle)

An Arduino-based autonomous robot that navigates its environment by detecting and avoiding obstacles. It uses a combination of an ultrasonic sensor for strategic path planning and infrared (IR) sensors for immediate emergency stops.

## Features
* **Hybrid Obstacle Detection:** Uses an HC-SR04 Ultrasonic sensor mounted on a Servo motor to scan for clear paths, alongside two LM393 IR sensors to cover blind spots.
* **Differential Drive System:** Utilizes two DC motors controlled by an L298N Motor Driver, allowing for precise maneuvers and spot-turns.
* **Architecture:** The code utilizes the `millis()` function instead of `delay()` to achieve pseudo-multitasking. This allows the robot to continuously update the LCD, read sensors, and track movement timeouts simultaneously.
* **Real-Time Data Display:** An I2C 16x2 LCD screen outputs the current distance to obstacles and the robot's status (e.g., "Moving...", "Scanning...", "IR ALERT!").
* **Safety Watchdog:** Implements a movement duration timeout to prevent the robot from moving blindly for too long without re-evaluating its surroundings.

## 🛠️ Hardware Components
* 1x Arduino UNO R3 (with Sensor Shield V5.0)
* 1x L298N Motor Driver
* 1x HC-SR04 Ultrasonic Sensor
* 2x LM393 IR Obstacle Sensors
* 1x SG-90 Micro Servo Motor
* 1x LCD 16x2 with I2C Module
* 2x DC Motors with Wheels & 1x Caster Wheel
* 1x Plexiglass Chassis
* Power Supply: 9V (6x 1.5V AA Batteries)

## Pin Configuration (Wiring)

| Component | Pin on Component | Arduino Pin |
| :--- | :--- | :--- |
| **L298N Driver** | ENA (Speed Left) | `D5` (PWM) |
| | ENB (Speed Right) | `D6` (PWM) |
| | IN1 (Dir Left Fwd) | `A3` (as Digital OUT) |
| | IN2 (Dir Left Back)| `A2` (as Digital OUT) |
| | IN3 (Dir Right Fwd)| `A1` (as Digital OUT) |
| | IN4 (Dir Right Back)| `A0` (as Digital OUT) |
| **Ultrasonic** | TRIG | `D8` |
| | ECHO | `D9` |
| **IR Sensors** | OUT (Left) | `D4` |
| | OUT (Right) | `D7` |
| **Servo Motor** | Signal | `D11` (PWM) |
| **LCD I2C** | SDA | `A4` |
| | SCL | `A5` |

## 💻 Software Requirements
This project is programmed using the **Arduino IDE**. The following libraries must be installed:
* `Wire.h` (Built-in)
* `Servo.h` (Built-in)
* `LiquidCrystal_I2C.h` (by Frank de Brabander or Marco Schwartz)

## 🚀 Installation & Usage
1. Clone this repository to your local machine:
   ```bash
   git clone [[https://github.com/KonstantinosLagos/obstacle-avoidance-ugv.git](https://github.com/yourusername/obstacle-avoidance-ugv.git)
](https://github.com/KonstantinosLagos/Obstacle_avoidance_vehicle.git)
