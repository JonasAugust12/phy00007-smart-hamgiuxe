# Smart-Hamgiuxe - IoT Smart Parking System

![System Diagram](docs/Diagram.png)

Smart parking management system with automated gate control, fire detection, and web monitoring.

## 🎥 Demo Videos & Screenshots

### System Demos

- [Parking Lot Demo](https://drive.google.com/drive/u/0/folders/10ofMhAusYKI4fNRivPvJcY8CxaRwe2DJ) - Vehicle entry/exit simulation
- [Siren Demo](https://drive.google.com/drive/u/0/folders/10ofMhAusYKI4fNRivPvJcY8CxaRwe2DJ) - Fire alarm system demonstration

### Hardware Circuits

![3D Parking Garage](docs/3DParkingGarage.png)
![Arduino Circuit](docs/ArduinoCircuit.jpg)
![ESP32 Circuit](docs/ESP32Circuit.jpg)

### Web Interface

![Login Page](docs/Login.png)
![Dashboard](docs/Dashboard.png)
![Parking Log](docs/ParkingLog.png)
![Chatbot](docs/Chatbot.png)
![Settings](docs/Setting.png)

### Notifications

![Email Report](docs/Email.jpg)
![PushSafer Notification](docs/PushSafer.png)

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

## Tech Stack

- **Hardware:** ESP32, Arduino UNO, Wokwi Simulator
- **Backend:** Node.js, Express.js, Firebase
- **Frontend:** HTML, CSS, JavaScript, EJS
- **AI:** Python Flask, PhoBERT
- **Services:** Cloudinary, Nodemailer, PushSafer

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
