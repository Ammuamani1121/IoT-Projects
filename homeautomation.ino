#include <ESP8266WiFi.h>

const char* ssid = "YOUR_WIFI_SSID";  // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password

WiFiServer server(80);

const int relay1 = D1;  // GPIO5
const int relay2 = D2;  // GPIO4

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);

  digitalWrite(relay1, HIGH);  // Default OFF state (Active LOW Relay)
  digitalWrite(relay2, HIGH);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("New Client Connected!");
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/relay1_on") != -1) {
    digitalWrite(relay1, LOW);
  } 
  if (request.indexOf("/relay1_off") != -1) {
    digitalWrite(relay1, HIGH);
  }
  if (request.indexOf("/relay2_on") != -1) {
    digitalWrite(relay2, LOW);
  }
  if (request.indexOf("/relay2_off") != -1) {
    digitalWrite(relay2, HIGH);
  }

  // Web Page Content
  client.println("HTTP/1.1