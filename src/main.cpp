#include <WiFi.h>
#include <WebServer.h>


const char* ssid = "ISU_HQ";  // Replace with your WiFi SSID
const char* password = "iServeu@8895"; // Replace with your WiFi password

WebServer server(80);
const int ledPin = 2;  // Built-in LED pin

void handleRoot() {
  String html = "<html><body>"
                "<h2>ESP32 LED Control</h2>"
                "<button onclick=\"toggleLED('on')\">Turn ON</button>"
                "<br><br><br>"
                "<button onclick=\"toggleLED('off')\">Turn OFF</button>"
                "<script>"
                "function toggleLED(state) {"
                " fetch('/led?state=' + state);"
                "}"
                "</script>"
                "</body></html>";
  server.send(200, "text/html", html);
}

void handleLEDControl() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(ledPin, HIGH);
  } else if (state == "off") {
    digitalWrite(ledPin, LOW);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin, OUTPUT);
  server.on("/", handleRoot);
  server.on("/led", handleLEDControl);
  server.begin();
  Serial.println("Web Server started.");
}

void loop() {
  server.handleClient();
}
