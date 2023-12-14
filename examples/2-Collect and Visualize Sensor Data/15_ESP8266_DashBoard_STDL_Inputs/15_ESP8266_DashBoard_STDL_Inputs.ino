#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <FS.h>
#include <Ticker.h>

//variableDeclaration
const char* ssid = "a";
const char* password = "aaaaaaaa";
String receivedPacket = "";
String incomingPacket = "";
bool newData = false;
bool WifiMode= false;

//defines
#define staMode true
#define ledPin D2

//instance(object)OfClasses...
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
Ticker timer1;


//functionDeclareation
void handleRoot();
void handleExport();
void handleNotFound();
void handdleIncomingSerialData();
void getData();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void logData(int S, long T, String D, float L, String NS, float O, String EW, int H);
void beatOnce();
bool TurnOnWiFi(bool mode);
bool TurnOnSD();
bool TurnOnWebSockets();
float readSensor1();
float readSensor2();


//functionUse...
void setup() {
  Serial.begin(115200);
  TurnOnWiFi(!staMode);
  TurnOnSD();
  TurnOnServer();
  TurnOnWebSockets();
  ledTicker.attach(onTime, toggleLED); // Start with the LED on-time
  timer1.attach(30, beatOnce);
  //timer1.attach(1, getData);
}

void loop() {
  server.handleClient();
  webSocket.loop();
  if (Serial.available()) handdleIncomingSerialData();
}

void handdleIncomingSerialData(){
  char receivedChar = Serial.read();
  // Assuming '\n' as the end of a transmission
  if (receivedChar == '\n') {
    receivedPacket = incomingPacket; // Copy the complete message
    incomingPacket = "";         // Clear the buffer
    newData = true;
  }
  else {
    incomingPacket += receivedChar;
  }
  
  // If new data is received, broadcast it via WebSocket
  if (newData) {
    webSocket.broadcastTXT(receivedPacket);
    Serial.println("Recieved Data: " + receivedPacket);
    StaticJsonDocument<500> doc;
    DeserializationError error = deserializeJson(doc, receivedPacket);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    //{"S":100,"T":133626,"D":051223,"L":32.62031,"NS":"N","O":51.66251,"EW":"E","H":59} 
    logData(doc["S"], doc["T"], doc["D"].as<String>(), doc["L"], doc["NS"], doc["O"], doc["EW"], doc["H"]); // Log data to SD card
    // logData(doc["S"], doc["T"], doc["D"], doc["L"], doc["NS"], doc["O"], doc["EW"], doc["H"]);
    // int S = doc["S"];
    // long T = doc["T"];
    // String D = doc["D"].as<String>();
    // float L = doc["L"]; // Longitude
    // String NS = doc["NS"].as<String>();
    // float O = doc["O"]; // Could be latitude or another coordinate
    // String EW = doc["EW"].as<String>();
    // int H = doc["H"];
    // OR:
    // int S = doc["S"];
    // long T = doc["T"];
    // const char* D = doc["D"]; // Treat D as a string
    // float L = doc["L"];
    // String NS = doc["NS"].as<String>();
    // float O = doc["O"];
    // String EW = doc["EW"].as<String>();
    // int H = doc["H"];
    //Serial.println(String(doc["sensor1"]) +" , "+ String(doc["sensor2"]));
    
    Serial.println(doc["D"].as<String>());
    //Serial.println(doc["D"]);
    Serial.println(String(doc["D"]));

    newData = false;
  }
}

void getData() {  
  float sensor1Value = readSensor1();
  float sensor2Value = readSensor2();

  //logData(sensor1Value, sensor2Value); // Log data to SD card

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
  } else if (path.endsWith(".ico")) {
    contentType = "image/ico";
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

void logData(int S, long T, String D, float L, String NS, float O, String EW, int H) {
  // logData(doc["S"], doc["T"], doc["D"], doc["L"], doc["NS"], doc["O"], doc["EW"], doc["H"]);
  // int S = doc["S"];
  // long T = doc["T"];
  // String D = doc["D"].as<String>();
  // float L = doc["L"]; // Longitude
  // String NS = doc["NS"].as<String>();
  // float O = doc["O"]; // Could be latitude or another coordinate
  // String EW = doc["EW"].as<String>();
  // int H = doc["H"];

  //File dataFile = SD.open("/Data/data.csv", "a");
  File dataFile = SD.open("/Data/data.csv", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Failed to open data.csv file for writing, So we can not log data for you! :(");
    return ;
  }
  // Format data as CSV
  dataFile.print(S);
  dataFile.print(',');
  dataFile.print(T);
  dataFile.print(',');
  dataFile.print(D);
  dataFile.print(',');
  dataFile.print(L, 6); // Print longitude with 6 decimal places
  dataFile.print(',');
  dataFile.print(NS);
  dataFile.print(',');
  dataFile.print(O, 6); // Assuming 'O' is another coordinate, print with 6 decimal places
  dataFile.print(',');
  dataFile.print(EW);
  dataFile.print(',');
  dataFile.println(H);

  dataFile.close();
  Serial.println("Data written to SD Card in CSV format.");
  
  // String dataString = String(sensor1Value) + "," + String(sensor2Value) + "\n";
  // dataFile.print(dataString);
  // dataFile.close();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:{
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connection from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      // You might want to send a message back to the client
      webSocket.sendTXT(num, "Connected to NodeMCU WebSocket Server");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      // You can process the text message here
      // For example, parse JSON data
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if (!error) {
        // Extract values
        //{"S":100,"T":133626,"D":051223,"L":32.62031,"NS":"N","O":51.66251,"EW":"E","H":59} 
        int sensor1 = doc["sensor1"]; // Extract sensor1 value
        int sensor2 = doc["sensor2"]; // Extract sensor2 value
        // Do something with sensor values
        Serial.printf("Sensor1: %d, Sensor2: %d\n", sensor1, sensor2);
      }
      else {
            Serial.println("Failed to parse JSON");
      }
      break;
    // case WStype_BIN:
    //   Serial.printf("[%u] Binary message received\n", num);
    //   break;
  }
}

float readSensor1() {
  return random(20, 30); // Dummy value for demonstration
}

float readSensor2() {
  return random(50, 70); // Dummy value for demonstration
}

bool TurnOnWiFi(bool mode){
  if(mode==staMode){
    while(1);
  }
  else{
    WiFi.begin(ssid, password);
    Serial.println("\n\nConnecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi Connected");
    Serial.print("\nConnected to: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    return 1;
  }
}

bool TurnOnSD(){
  if (!SD.begin(D8)) { // CS pin is connected to D8
    Serial.println("SD card initialization failed!");
    return 0;
  }
  Serial.println("SD card initialized.");
  File dataFile = SD.open("/Data/data.csv", FILE_WRITE);

  // Check if the file is new or empty and write the header
  if (dataFile.size() == 0) {
    dataFile.println("SN,Time,Date,latitude,N/S,Longitude,E/W,Heart Rate");
  }
  
  dataFile.close();
  return 1;
}

bool TurnOnServer(){
  server.on("/", HTTP_GET, handleRoot);
  server.on("/export", HTTP_GET, handleExport);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  return 1;
}

bool TurnOnWebSockets(){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("webSocket started");
  return 1;
}

void beatOnce(){
    Serial.print("🤍");
    //digitalWrite(ledPin, !digitalRead(ledPin));
}


