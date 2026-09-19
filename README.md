# Smart Pothole Detection System

An IoT and AI-based system for detecting road potholes and anomalies using **YOLO, ESP32-CAM, MPU6050, NEO-6M GPS, and Telegram alerts**.

The project combines computer vision-based pothole detection with sensor-based road anomaly monitoring to support road-condition identification and location reporting.

## Project Overview

Poor road conditions and potholes can affect vehicle safety and ride quality. This project explores an automated approach for detecting road anomalies and providing location-based alerts.

The system consists of two main parts:

1. **AI-Based Pothole Detection**
   - YOLO object detection model
   - Detects potholes from road images/video
   - Trained model stored as `best.pt`

2. **IoT-Based Road Anomaly Detection**
   - ESP32-CAM used as the controller
   - MPU6050 measures acceleration and road shocks
   - NEO-6M provides GPS coordinates
   - Buzzer provides a local indication
   - Wi-Fi enables Telegram notifications

## System Architecture

```text
               ROAD CONDITION
                      |
          +-----------+-----------+
          |                       |
          v                       v
    Camera / Image             MPU6050
          |                       |
          v                       v
      YOLO Model            ESP32-CAM
          |                       |
          |                  NEO-6M GPS
          |                       |
          v                       v
   Pothole Detection       Road Shock Detection
          |                       |
          +-----------+-----------+
                      |
                      v
               Alert / Location
                      |
                      v
                  Telegram
```

## Hardware Components

- ESP32-CAM
- MPU6050 Accelerometer/Gyroscope
- NEO-6M GPS Module
- Buzzer
- USB-to-TTL Converter
- Jumper Wires
- Breadboard / Prototype Connections
- Power Supply

## Software & Technologies

- Python
- Arduino IDE
- Google Colab
- Ultralytics YOLO
- OpenCV
- PyTorch
- ESP32
- Telegram Bot API
- GitHub

## AI Model

The computer-vision component uses a YOLO-based object detection model trained on pothole images.

The trained weights are available at:

```text
AI_Model/best.pt
```

The training notebook is available in:

```text
AI_Model/Pothole_Detection_YOLO.ipynb
```

## IoT Operation

The MPU6050 measures acceleration along the X, Y and Z axes.

The resultant acceleration is calculated as:

```text
A = sqrt(Ax² + Ay² + Az²)
```

A road-shock value is estimated relative to gravitational acceleration.

When the configured threshold is exceeded, the system can:

- Detect a significant road shock/anomaly
- Activate the buzzer
- Obtain available GPS coordinates
- Generate a location link
- Send an alert through Telegram when Wi-Fi is available

> MPU6050 shock detection indicates a road anomaly and should not by itself be interpreted as definitive pothole classification. The YOLO model provides the visual pothole-detection component.

## Pin Connections

| Module | Module Pin | ESP32-CAM |
|---|---|---|
| MPU6050 | VCC | 3.3V |
| MPU6050 | GND | GND |
| MPU6050 | SDA | GPIO 13 |
| MPU6050 | SCL | GPIO 15 |
| NEO-6M | VCC | 5V |
| NEO-6M | GND | GND |
| NEO-6M | TX | GPIO 14 |
| Buzzer | + | GPIO 4 |
| Buzzer | - | GND |

## Repository Structure

```text
Smart-Pothole-Detection-System/
│
├── AI_Model/
│   ├── Pothole_Detection_YOLO.ipynb
│   └── best.pt
│
├── Arduino/
│   └── Smart_Pothole_Detection.ino
│
├── Documentation/
│
├── Hardware/
│
├── Mockups/
│
├── Results/
│
├── README.md
└── requirements.txt
```

## Running the AI Model

Install the required Python packages:

```bash
pip install -r requirements.txt
```

A basic Ultralytics inference example is:

```python
from ultralytics import YOLO

model = YOLO("AI_Model/best.pt")

results = model.predict(
    source="road_image.jpg",
    save=True
)
```

## ESP32 Configuration

Before uploading the Arduino firmware, replace the placeholder credentials locally:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

#define BOT_TOKEN "YOUR_TELEGRAM_BOT_TOKEN"
#define CHAT_ID "YOUR_TELEGRAM_CHAT_ID"
```

**Never commit real Wi-Fi passwords, bot tokens, API keys, or other private credentials to a public repository.**

## Results

Actual YOLO detection outputs and experimental screenshots are stored in:

```text
Results/
```

Hardware prototype photographs are stored in:

```text
Hardware/
```

## Expected Output Mockups

Conceptual Telegram/GPS output demonstrations are stored separately in:

```text
Mockups/
```

These images are illustrative and are not presented as actual experimental results.

## Future Improvements

- Integrate visual and sensor detections into a unified pipeline
- Perform additional real-road testing
- Tune road-shock thresholds using experimental data
- Improve pothole severity estimation
- Store detected locations in a database
- Develop a GIS-based road-condition dashboard
- Expand the pothole image dataset
- Optimize the model for edge deployment

## Applications

- Smart road monitoring
- Road maintenance support
- Smart-city infrastructure
- Road-condition mapping
- Automated pothole identification

## Project Status

The YOLO-based pothole detection model has been trained and tested. Hardware integration and road-anomaly monitoring are being developed and validated as part of the IoT component.

## Disclaimer

This is an academic prototype. Detection accuracy and sensor thresholds depend on the dataset, mounting position, road conditions, vehicle dynamics, GPS availability, and hardware configuration.
