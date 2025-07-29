#include <ESP8266WiFi.h>
#include <DHT.h>
#include <FirebaseArduino.h>
// codex/add-dht11-sensor-logging-to-firebase
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
=======
// main

#define DHTPIN 2          // D4 on NodeMCU boards
#define DHTTYPE DHT11

// Replace with your network credentials and Firebase settings
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* firebaseHost = "YOUR_FIREBASE_HOST"; // e.g. your-project.firebaseio.com
const char* firebaseAuth = "YOUR_DATABASE_SECRET";

DHT dht(DHTPIN, DHTTYPE);

 // ulxb08-codex/add-dht11-sensor-logging-to-firebase
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

=======
// main
void setup() {
  Serial.begin(115200);
  delay(100);

  dht.begin();

// ulxb08-codex/add-dht11-sensor-logging-to-firebase
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;) {}
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting...");
  display.display();

=======
// main
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

//ulxb08-codex/add-dht11-sensor-logging-to-firebase
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi connected");
  display.display();

=======
//  Firebase.begin(firebaseHost, firebaseAuth);
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C, Humidity: ");
  Serial.print(h);
  Serial.println(" %");
//-codex/add-dht11-sensor-logging-to-firebase
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(t);
  display.println(" C");
  display.print("Hum:  ");
  display.print(h);
  display.println(" %");
  display.display();

=======
// main
  Firebase.setFloat("/dht11/temperature", t);
  if (Firebase.failed()) {
    Serial.print("setFloat failed: ");
    Serial.println(Firebase.error());
    return;
  }

  Firebase.setFloat("/dht11/humidity", h);
  if (Firebase.failed()) {
    Serial.print("setFloat failed: ");
    Serial.println(Firebase.error());
    return;
  }

  delay(5000); // send data every 5 seconds
}
