# Smart-Hamgiuxe - IoT Smart Parking System

<div align="center">
<img src="docs/Diagram.png" width="650">
</div>

Smart parking management system with automated gate control, fire detection, and web monitoring.

## 🎥 Demo Videos & Screenshots

### System Demos

- [Parking Lot Demo](https://drive.google.com/file/d/164KKhLscCx9l8UL9_osZAa_X4LqKg5fX/view?usp=sharing) - Vehicle entry/exit simulation
- [Siren Demo](https://drive.google.com/file/d/11ITv-zhT1ou3GHXgi8XvYJygERg_G8v5/view?usp=sharing) - Fire alarm system demonstration

### Hardware Circuits

<div align="center">
<img src="docs/3DParkingGarage.png" width="650">
<br><br>
<img src="docs/ArduinoCircuit.jpg" width="650">
<br><br>
<img src="docs/ESP32Circuit.jpg" width="650">
</div>

### Web Interface

<div align="center">
<img src="docs/Login.png" width="650">
<br><br>
<img src="docs/Dashboard.png" width="650">
<br><br>
<img src="docs/ParkingLog.png" width="650">
<br><br>
<img src="docs/Chatbot.png" width="650">
<br><br>
<img src="docs/Setting.png" width="650">
</div>

### Notifications

<div align="center">
<img src="docs/Email.jpg" width="650">
<br><br>
<img src="docs/PushSafer.png" width="650">
</div>

## Features

- **Automated Gate:** Ultrasonic sensors + servo motor for vehicle entry/exit
- **Fire Detection:** MQ2 smoke sensor + temperature monitoring with alarms
- **Parking Monitoring:** IR sensors + RGB LEDs for slot availability
- **Web Dashboard:** Real-time monitoring with user authentication
- **AI Chatbot:** Vietnamese language support for system queries
- **Notifications:** Email reports + push notifications via PushSafer

## System Overview

```
Arduino UNO ──── ESP32 DevKit ──── Web Dashboard
(Parking Slots)  (Gate + Fire)     (Monitoring)
     │                 │                │
     └─────────────────┼────────────────┘
                       │
                  Firebase
```

## Hardware Setup

### ESP32 Components:

- MQ2 Smoke Sensor (Pin 36), Temperature Sensor (Pin 32)
- Ultrasonic Sensors: Entry (33,34), Exit (23,39)
- Servo Motor (Pin 16), LCD (21,22), Buzzer (Pin 5)
- Emergency Button (Pin 35), LED Alarm (Pin 18)

### Arduino Components:

- 3x IR Sensors (Pins 2,4,7), 3x RGB LEDs (Pins 3,5,6,9,10,11)
- RTC Clock (A4,A5)

# Tech Stack

## Hardware

![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)
![Wokwi](https://img.shields.io/badge/Wokwi-4FC3F7?style=for-the-badge&logo=arduino&logoColor=white)

## Frontend

![HTML5](https://img.shields.io/badge/HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white)
![CSS3](https://img.shields.io/badge/CSS3-1572B6?style=for-the-badge&logo=css3&logoColor=white)
![JavaScript](https://img.shields.io/badge/JavaScript-F7DF1E?style=for-the-badge&logo=javascript&logoColor=black)
![EJS](https://img.shields.io/badge/EJS-B4CA65?style=for-the-badge&logo=ejs&logoColor=black)

## Backend

![Node.js](https://img.shields.io/badge/Node.js-43853D?style=for-the-badge&logo=node.js&logoColor=white)
![Express.js](https://img.shields.io/badge/Express.js-404D59?style=for-the-badge&logo=express&logoColor=white)
![Firebase](https://img.shields.io/badge/Firebase-039BE5?style=for-the-badge&logo=Firebase&logoColor=white)

## AI & Machine Learning

![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Flask](https://img.shields.io/badge/Flask-000000?style=for-the-badge&logo=flask&logoColor=white)
![PyTorch](https://img.shields.io/badge/PyTorch-EE4C2C?style=for-the-badge&logo=pytorch&logoColor=white)

## Cloud Services

![Cloudinary](https://img.shields.io/badge/Cloudinary-3448C5?style=for-the-badge&logo=cloudinary&logoColor=white)
![Gmail](https://img.shields.io/badge/Gmail-D14836?style=for-the-badge&logo=gmail&logoColor=white)
![PushSafer](https://img.shields.io/badge/PushSafer-FF6B35?style=for-the-badge&logo=pusher&logoColor=white)

## Development Tools

![PlatformIO](https://img.shields.io/badge/PlatformIO-FF7F00?style=for-the-badge&logo=platformio&logoColor=white)
![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)

## Quick Start

### 1. Hardware Simulation (Wokwi)

```bash
# ESP32 Circuit
cd src/smart-hamgiuxe
pio run --target upload

# Arduino Circuit
cd src/smart-hamgiuxe-arduino
pio run --target upload
```

### 2. Web Application

```bash
# Install dependencies
cd webSrc
npm install

# Start server
npm start
```

### 3. AI Chatbot

```bash
cd webSrc/views/chatbot
pip install -r requirements.txt
python app.py
```

Access the dashboard at `http://localhost:3000`

## Environment Variables

Create `.env` file in `webSrc/`:

```env
PORT=3000
FIREBASE_SERVICE_ACCOUNT_PATH=./phy00007-smart-hamgiuxe-XXXX-firebase-adminsdk-XXXX-XXXXX.json
FIREBASE_DATABASE_URL=https://your-project.firebaseio.com
CLOUDINARY_CLOUD_NAME=your_cloudinary_name
CLOUDINARY_API_KEY=your_cloudinary_key
CLOUDINARY_API_SECRET=your_cloudinary_secret
CLIENT_ID=your_gmail_oauth_client_id
CLIENT_SECRET=your_gmail_oauth_client_secret
REDIRECT_URI=https://developers.google.com/oauthplayground
REFRESH_TOKEN=your_gmail_refresh_token
PUSHSAFER_KEY=your_pushsafer_api_key
```

### Firebase Service Account File

The file `phy00007-smart-hamgiuxe-XXXX-firebase-adminsdk-XXXX-XXXXX.json` contains Firebase Admin SDK credentials with this structure:

```json
{
  "type": "service_account",
  "project_id": "phy00007-smart-hamgiuxe-XXXX",
  "private_key_id": "your_private_key_id",
  "private_key": "-----BEGIN PRIVATE KEY-----\nyour_private_key\n-----END PRIVATE KEY-----\n",
  "client_email": "firebase-adminsdk-XXXX@phy00007-smart-hamgiuxe-XXXX.iam.gserviceaccount.com",
  "client_id": "your_client_id",
  "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  "token_uri": "https://oauth2.googleapis.com/token",
  "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/firebase-adminsdk-XXXX%40phy00007-smart-hamgiuxe-XXXX.iam.gserviceaccount.com",
  "universe_domain": "googleapis.com"
}
```

**Required for:**

- Firebase: Database and authentication
- Cloudinary: Image storage for user avatars
- Gmail OAuth: Email notifications
- PushSafer: Push notifications

## Project Structure

```
├── src/
│   ├── smart-hamgiuxe/          # ESP32 main controller
│   └── smart-hamgiuxe-arduino/  # Arduino parking sensors
├── webSrc/                      # Web application
│   ├── app.js                   # Express server
│   ├── controllers/             # Business logic
│   ├── views/                   # EJS templates
│   │   └── chatbot/             # AI chatbot (Flask)
│   └── config/                  # Firebase, Cloudinary configs
├── docs/                        # Documentation & media files
```
