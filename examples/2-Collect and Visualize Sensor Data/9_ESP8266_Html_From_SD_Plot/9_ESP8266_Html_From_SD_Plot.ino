#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <SD.h>
#include <FS.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
void handleRoot();
float readSensor1();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void logData(float sensor1Value, float sensor2Value);
void handleExport();
float readSensor2();


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if (!SD.begin(D8)) { // CS pin is connected to D8
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/export", HTTP_GET, handleExport);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();

  float sensor1Value = readSensor1();
  float sensor2Value = readSensor2();

  logData(sensor1Value, sensor2Value); // Log data to SD card

  String jsonData = "{\"sensor1\":" + String(sensor1Value) + ", \"sensor2\":" + String(sensor2Value) + "}";
  webSocket.broadcastTXT(jsonData);

  delay(5000); // Send data every 5 seconds
}

void handleRoot() {
      File file = SD.open("/graph.html"); // Make sure the path matches the location on the SD card
    if (file) {
      server.streamFile(file, "text/html");
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  }

void handleExport() {
  File dataFile = SD.open("/Data/data.csv", "r");
  if (!dataFile) {
    server.send(404, "text/plain", "Data file not found");
    return;
  }
  
  server.streamFile(dataFile, "text/csv");
  dataFile.close();
}

void logData(float sensor1Value, float sensor2Value) {
  File dataFile = SD.open("/Data/data.csv", "a");
  if (!dataFile) {
    Serial.println("Failed to open data file for writing");
    return;
  }
  
  String dataString = String(sensor1Value) + "," + String(sensor2Value) + "\n";
  dataFile.print(dataString);
  dataFile.close();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // ... same as before ...
}

float readSensor1() {
  return random(20, 30); // Dummy value for demonstration
}

float readSensor2() {
  return random(50, 70); // Dummy value for demonstration
}
