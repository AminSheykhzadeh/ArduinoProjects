#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SD.h>
#include <SPI.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Initialize SD card
  if (!SD.begin(D8)) {
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
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void handleNotFound() {
  String path = server.uri(); // Get the requested path
  File file = SD.open(path, "r"); // Try to open the file from SD card

  if (!file) {
    // If the file doesn't open, send 404 not found
    Serial.println("File not found: " + path);
    server.send(404, "text/plain", "404: Not Found");
    return;
  }

  // Get the content type
  String contentType = "text/plain";
  if (path.endsWith(".html")) {
    contentType = "text/html";
  } else if (path.endsWith(".css")) {
    contentType = "text/css";
  } else if (path.endsWith(".js")) {
    contentType = "application/javascript";
  } else if (path.endsWith(".png")) {
    contentType = "image/png";
  } else if (path.endsWith(".jpg")) {
    contentType = "image/jpeg";
  } else if (path.endsWith(".gif")) {
    contentType = "image/gif";
  } 

  // Stream the file
  server.streamFile(file, contentType);
  file.close();
}

void loop() {
  server.handleClient();
}
