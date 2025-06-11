# EvilPortal for ESP8266 (NodeMCU 1.0 - ESP-12E)

![ESP8266](https://img.shields.io/badge/Board-ESP8266%20NodeMCU%201.0-orange)
![Purpose](https://img.shields.io/badge/Purpose-Educational-blue)
![License](https://img.shields.io/badge/License-MIT-green)

---

## 📘 Description

**EvilPortal** is a lightweight captive portal project for the ESP8266 NodeMCU 1.0 (ESP-12E module), designed to simulate a Wi-Fi login page that captures credentials entered by users. This project is intended **strictly for educational and penetration testing training purposes only**.

Once deployed, the ESP8266 acts as a Wi-Fi access point. When a user connects, they are redirected to a fake login page (captive portal). Credentials entered on this page are captured and stored locally.

---

## ⚠️ Disclaimer

> **This project is intended for educational purposes only.**
>
> Do **not** use this on networks you do not own or have explicit written permission to test. Unauthorized use of this tool can be considered illegal and unethical.
>
> The developers are **not responsible for any misuse or damages** caused by using this software.

---

## 🧰 Features

- Captive portal for Wi-Fi login phishing
- Stores submitted credentials locally
- Simple OLED display support (optional)
- Captured credentials accessible via local endpoint

---

## 🛠 Hardware Requirements

- ESP8266 NodeMCU 1.0 (ESP-12E)
- Micro USB cable
- (Optional) OLED Display (I2C)

---

## 💻 Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP8266 board package installed via Boards Manager
- Required Arduino libraries:
  - `ESP8266WiFi.h`
  - `DNSServer.h`
  - `ESP8266WebServer.h`
  - `U8g2lib.h` (for OLED support)

---

## 📂 Project Structure

