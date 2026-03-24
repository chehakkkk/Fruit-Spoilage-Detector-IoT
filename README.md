# 🍎 Fruit Spoilage Detector using IoT

## 📌 Abstract

The **Fruit Spoilage Detector** is an IoT-based system designed to monitor and detect the spoilage level of fruits in real time. The system uses gas sensors and environmental sensors to analyze gases released during different stages of fruit ripening and spoilage. Based on the detected parameters, it provides alerts and data visualization to the user.

---

## 🎯 Objectives

* Detect fruit spoilage using gas emission analysis
* Monitor environmental conditions like temperature and humidity
* Provide real-time alerts to users
* Visualize data trends using cloud platforms
* Improve food storage and reduce wastage

---

## 🛠️ Hardware Components

| Component     | Description                                     |
| ------------- | ----------------------------------------------- |
| ESP32         | Microcontroller for processing and connectivity |
| Arduino Board | Used for interfacing sensors                    |
| DHT11 Sensor  | Measures temperature and humidity               |
| MQ2 Sensor    | Detects combustible gases                       |
| MQ135 Sensor  | Detects air quality and harmful gases           |

---

## 💻 Software & Technologies Used

* Embedded C / Arduino IDE
* IoT Communication (WiFi using ESP32)
* ThingSpeak (Cloud platform for data visualization)
* Telegram Bot API (User notifications)

---

## ⚙️ System Architecture

1. Sensors collect environmental and gas data
2. Data is processed by ESP32
3. Data is uploaded to ThingSpeak cloud
4. Telegram bot sends alerts when spoilage threshold is crossed
5. User monitors real-time data and receives notifications

---
## Libraries Used

1. Adafruit Unified sensor
2. ArduinoJson
3. DHT sensor library
4. ESP Mail Client
5. IRremote
6. UniversalTelegramBot
---
## 🔬 Working Principle

Fruits release specific gases (like ethylene, ammonia, etc.) during ripening and spoilage.

* MQ2 and MQ135 sensors detect gas concentration levels
* DHT11 monitors temperature and humidity conditions
* When gas levels exceed predefined thresholds:

  * System identifies spoilage stage
  * Sends alert via Telegram bot
  * Logs data to ThingSpeak

---

## 📊 Features

* ✅ Real-time spoilage detection
* ✅ Gas concentration monitoring
* ✅ Temperature & humidity tracking
* ✅ Cloud-based data visualization
* ✅ Instant Telegram notifications
* ✅ Low-cost and scalable solution

---

## 🔐 Security & Data Protection

Sensitive credentials such as:

* WiFi SSID & Password
* ThingSpeak API Keys
* Telegram Bot Token

are stored in a separate file:

```bash
secrets.h
```

This file is excluded using `.gitignore` to prevent exposure on public repositories.

---

## 📁 Project Structure

```
Fruit-Spoilage-Detector-IoT/
│── project.ino        # Main Arduino code
│── secrets.h          # Sensitive credentials (ignored)
│── .gitignore         # Hides secrets file
│── README.md          # Project documentation
```

## 📈 Future Enhancements

* AI-based spoilage prediction
* Mobile app integration
* Multi-fruit classification
* Improved sensor calibration
* Automated storage system

---

## 👩‍💻 Author

**Chehak Makker**

---

## 📜 License

This project is for academic and educational purposes.
