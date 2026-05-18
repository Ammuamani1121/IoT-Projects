#include <SoftwareSerial.h>

#define TRIG_PIN 9
#define ECHO_PIN 10
#define RAIN_PIN A0

#define BUZZER 6
#define LED 7

// ESP8266 WiFi Pins
SoftwareSerial esp(2, 3);  // TX, RX

String API = "YOUR_THINGSPEAK_API_KEY";
String WIFI_NAME = "YOUR_WIFI_NAME";
String WIFI_PASS = "YOUR_WIFI_PASSWORD";

#define SAFE_LEVEL 50
#define WARNING_LEVEL 30
#define DANGER_LEVEL 15

long duration;
float distance;
int rainValue;
String floodStatus;

void setup() {
  Serial.begin(9600);
  esp.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);

  connectWiFi();
}

void loop() {

  distance = measureWaterLevel();
  rainValue = analogRead(RAIN_PIN);

  checkFloodStatus(distance);

  Serial.print("Water Level: ");
  Serial.println(distance);

  Serial.print("Rain: ");
  Serial.println(rainValue);

  Serial.print("Status: ");
  Serial.println(floodStatus);

  sendToCloud(distance, rainValue, floodStatus);

  delay(15000);  // ThingSpeak limit
}

// Ultrasonic Sensor Function
float measureWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  float dist = duration * 0.034 / 2;

  return dist;
}

// Flood Classification
void checkFloodStatus(float level) {

  if (level > SAFE_LEVEL) {
    floodStatus = "SAFE";
    digitalWrite(BUZZER, LOW);
    digitalWrite(LED, LOW);
  }
  else if (level <= SAFE_LEVEL && level > WARNING_LEVEL) {
    floodStatus = "WARNING";
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER, LOW);
  }
  else {
    floodStatus = "DANGER";
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER, HIGH);
  }
}

// ESP8266 WiFi Connect
void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  esp.println("AT");
  delay(2000);

  esp.println("AT+CWMODE=1");
  delay(1000);

  esp.println("AT+CWJAP=\"" + WIFI_NAME + "\",\"" + WIFI_PASS + "\"");
  delay(6000);

  Serial.println("Connected to WiFi.");
}

// Send Data to ThingSpeak
void sendToCloud(float water, int rain, String status) {

  String cmd = "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80";
  esp.println(cmd);
  delay(2000);

  String data = "GET /update?api_key=" + API +
                "&field1=" + String(water) +
                "&field2=" + String(rain) +
                " HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";

  esp.print("AT+CIPSEND=");
  esp.println(data.length());
  delay(1000);

  esp.print(data);
  delay(2000);

  esp.println("AT+CIPCLOSE");
}