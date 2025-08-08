# 💊 Medibox – IoT-Based Medicine Reminder and Monitoring System

This project implements an **ESP32-based Medibox** that reminds users to take their medicine via an alarm system and monitors environmental conditions like **temperature**, **humidity**, and **light intensity**. It connects to the **internet via Wi-Fi**, synchronizes time using **NTP**, and uses **MQTT (via Node-RED)** to remotely monitor and control parameters such as **sampling interval**, **sending interval**, and **servo motor angle**.

---

## 🚀 Features

- ⏰ **Real-time Clock Synchronization** via NTP
- 🔔 **Multi-Alarm System** with LED & Buzzer Notifications
- 📊 **Environment Monitoring**
  - Temperature (DHT22)
  - Humidity (DHT22)
  - Light Intensity (LDR)
- 🛰️ **Remote MQTT Control via Node-RED**
  - Set sampling & sending intervals
  - Control servo motor angle
- 🧠 **OLED Interface & Button Menu**
  - Set time offset from UTC
  - Set/Remove alarms
  - View active alarms
- 🧪 **Safety Alert System**
  - Warnings for out-of-range temperature and humidity via LED and screen

---

## 📦 Hardware Components

| Component        | Quantity |
|------------------|----------|
| ESP32 Dev Board   | 1        |
| DHT22 Sensor      | 1        |
| LDR (Light Sensor)| 1        |
| OLED Display (SSD1306) | 1  |
| Buzzer            | 1        |
| LEDs              | 2        |
| Push Buttons      | 4        |
| Servo Motor       | 1        |
| Resistors         | as needed|
| Breadboard & Wires| as needed|

---
---

## 🔌 Circuit Diagram

Wiring diagram is included as `diagram.jsin` .
---

## 🧠 How It Works

### 1. Time Synchronization
- Uses **NTP server** to sync local time based on a UTC offset.
- Offset can be configured through the menu system.


### 2. Alarm System
- Supports 2 configurable alarms.
- Each alarm triggers a buzzer and LED with snooze or cancel options via buttons.

### 3. Environmental Monitoring
- Monitors temperature & humidity via **DHT22**
- Calculates light intensity via **LDR**
- Displays warning messages on OLED and sets LED indicator if values exceed safe thresholds.

### 4. MQTT Communication
- Connects to `test.mosquitto.org`
- Publishes:
  - `TEMP` → temperature data
  - `Light intensity` → average light data
- Subscribes:
  - `sampling_interval` → time between light sensor readings
  - `sending_interval` → time between MQTT publishes
  - `MINIMUM-SERVO-ANGLE` → adjust servo motor angle remotely

### 5. Node-RED Dashboard
- Use the provided `dashboard.json` to visualize:
  - Live temperature and light intensity charts
  - Control sliders for:
    - Sampling interval
    - Sending interval
    - Servo angle

---

## 📲 MQTT Topics

| Topic               | Direction | Data Type | Description                     |
|---------------------|-----------|-----------|---------------------------------|
| `TEMP`              | Publish   | `float`   | Current temperature (°C)       |
| `Light intensity`   | Publish   | `float`   | Normalized light intensity     |
| `sampling_interval` | Subscribe | `float`   | Time between light readings (s)|
| `sending_interval`  | Subscribe | `float`   | Time between data sends (s)    |
| `MINIMUM-SERVO-ANGLE`| Subscribe| `float`   | Servo motor angle              |

---


## 🧪 Screenshots


---![Dashboard.png]("https://github.com/Shanmathu21/Medi_Box-Smart-IoT-Medication-Monitoring/blob/main/Images/Dashboard.png
")

![Wowki.png]("https://github.com/Shanmathu21/Medi_Box-Smart-IoT-Medication-Monitoring/blob/main/Images/Wowki.png")

![NodeRed.png]("https://github.com/Shanmathu21/Medi_Box-Smart-IoT-Medication-Monitoring/blob/main/Images/NodeRed.png")
## 🛠️ Future Improvements

- Add cloud storage (Firebase/ThingSpeak)
- Add medicine slot detection (via IR sensors)
- Battery backup support
- SMS or app-based alarm alerts

---
