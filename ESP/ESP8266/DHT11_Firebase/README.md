# DHT11 Firebase Logger (ESP8266)

This sketch demonstrates how to read temperature and humidity data from a DHT11 sensor and upload the values to Firebase using an ESP8266 board. It is intended for the Arduino IDE.

## Required libraries

- **ESP8266WiFi** (included with ESP8266 core)
- **DHT sensor library** by Adafruit
- **FirebaseArduino** library

Install these libraries through the Arduino Library Manager before compiling the example.

## Wiring

- DHT11 data pin -> ESP8266 GPIO2 (D4 on NodeMCU)
- DHT11 VCC pin -> 3.3V
- DHT11 GND pin -> GND

## Configuration

Replace the placeholders in `DHT11_Firebase.ino` with your WiFi credentials and Firebase settings:

```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* firebaseHost = "YOUR_FIREBASE_HOST"; // e.g. your-project.firebaseio.com
const char* firebaseAuth = "YOUR_DATABASE_SECRET";
```

## Running the example

Upload the sketch to your ESP8266 board. Once connected to WiFi, the device will periodically send temperature and humidity readings to the `dht11` path in your Firebase database.
