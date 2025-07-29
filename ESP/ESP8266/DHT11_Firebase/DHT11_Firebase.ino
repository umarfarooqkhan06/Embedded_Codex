#include <ESP8266WiFi.h>
#include <DHT.h>
#include <FirebaseArduino.h>

#define DHTPIN 2          // D4 on NodeMCU boards
#define DHTTYPE DHT11

// Replace with your network credentials and Firebase settings
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* firebaseHost = "YOUR_FIREBASE_HOST"; // e.g. your-project.firebaseio.com
const char* firebaseAuth = "YOUR_DATABASE_SECRET";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(100);

  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(firebaseHost, firebaseAuth);
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
