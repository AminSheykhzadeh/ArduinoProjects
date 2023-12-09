#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <SD.h>
#include <FS.h>
#include <Ticker.h>

//variableDeclare...
const char* ssid = "a";
const char* password = "aaaaaaaa";

//instance(object)OfClasses...
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
Ticker timer1;

//functionDeclare...
void handleRoot();
void handleExport();
void handleNotFound();
void getData();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void logData(float sensor1Value, float sensor2Value);
float readSensor1();
float readSensor2();

//functionUse...
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi Connected");
  Serial.print("\nConnected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");

  if (!SD.begin(D8)) { // CS pin is connected to D8
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/export", HTTP_GET, handleExport);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("webSocket started");

  timer1.attach(1, getData);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

void getData() {  
  float sensor1Value = readSensor1();
  float sensor2Value = readSensor2();

  logData(sensor1Value, sensor2Value); // Log data to SD card

  String jsonData = "{\"sensor1\":" + String(sensor1Value) + ", \"sensor2\":" + String(sensor2Value) + "}";
  webSocket.broadcastTXT(jsonData);
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

void handleExport() {
  File dataFile = SD.open("/Data/data.csv", "r");
  if (!dataFile) {
    server.send(404, "text/plain", "Data file not found, So we can not handle export for you! :(");
    return;
  }
  
  server.streamFile(dataFile, "text/csv");
  dataFile.close();
}

void logData(float sensor1Value, float sensor2Value) {
  File dataFile = SD.open("/Data/data.csv", "a");
  if (!dataFile) {
    Serial.println("Failed to open data.csv file for writing, So we can not log data for you! :(");
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
