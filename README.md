Live Weather Tracker

A real-time weather tracking system built using ESP32, Firebase Firestore, and a DHT11 sensor. This system reads temperature and humidity data at regular intervals and stores it in the cloud, which can be visualized using a web interface.

Features
Real-time temperature and humidity monitoring
ESP32 + DHT11 hardware integration
Firebase Firestore cloud storage
Web-based frontend

Hardware Used
ESP32 WROOM 32
DHT11 sensor (to measure both temperature and humidity)
Breadboard + jumper wires
Optional: USB power bank or adapter

🔧 Getting Started
1. Firebase Setup
Go to Firebase Console
Create a new project (e.g., weather-tracker)
Enable Firestore in test mode
Add a new document under a collection (e.g., records) with fields: temperature, humidity, humidity_quality, time
Copy your Firebase config (apiKey, projectId, etc.)

2. ESP32 Code Setup
Clone this repository
Open the Arduino code in Arduino IDE
Install dependencies:
DHT sensor library by Adafruit
ArduinoJson
Enter your:
WiFi SSID and password
Firebase API key and project ID
Document path (e.g., collection/document)
Flash code to ESP32

3. Web Frontend Setup
Open the index.html file in a browser
Add your Firebase config in the script
It will fetch and show weather data in real time

Feel free to fork this repo and build upon it for your own weather station!

