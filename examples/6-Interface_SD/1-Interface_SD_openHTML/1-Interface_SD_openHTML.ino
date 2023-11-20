#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SD.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  
  // Initialize SD card
  if (!SD.begin(D2)) {
    Serial.println("Card Mount Failed");
    return;
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  // Starting the server
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
}

void handleRoot() {
  File file = SD.open("/index.html"); // Make sure the path matches the location on the SD card
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

void loop() {
  server.handleClient();
}
