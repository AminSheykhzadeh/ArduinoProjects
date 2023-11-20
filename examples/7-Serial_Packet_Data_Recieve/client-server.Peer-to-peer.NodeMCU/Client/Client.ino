/*------------------------------------------------------------------------------
  09/10/2023
  Author: Amin,Sheykhzadeh
  Platforms: ESP8266,Arduino
  Language: C++/Arduino
  File: client-server.Peer-to-peer.NodeMCU/Client.ino
  ------------------------------------------------------------------------------
  Description: 
  Client-side code for YouTube video demonstrating how to communicate over WiFi
  between two ESP8266
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
// Setup the client to send sensor data to the server
#include <ESP8266WiFi.h>

// Initialize sensor parameters
float volts = 0.0, temperatureC = 0.0;

// Initialize network parameters
const char* ssid = "HILA-EPTSV_Server";
const char* password = "........";
const char* host = "192.168.11.4"; // as specified in Server.ino

// Set up the client objet
WiFiClient client;

// Configure deep sleep in between measurements
const int sleepTimeSeconds = 1;

void setup() {
  // Configure the sensor pin (optional)
  pinMode(A0, INPUT);
  // Connect to the server
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address (AP): ");
  Serial.println(WiFi.localIP());
  
}

void loop() {
// Get a measurement from the sensor
  volts = analogRead(A0)*3.3/1024; // 10-Bit ADC on a 3.3VDC board
  temperatureC = temperatureC+0.1;
  Serial.print("Value: "); Serial.println(temperatureC);
  // Connect to the server and send the data as a URL parameter
  if(client.connect(host,80)) {
    String url = "/update?value=";
    url += String(temperatureC);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host +  "\r\n" + 
                 "Connection: keep-alive\r\n\r\n"); // minimum set of required URL headers
    delay(10);
    // Read all the lines of the response and print them to Serial
    Serial.println("Response: ");
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  //Serial.println("ESP8266 in sleep mode");
  //ESP.deepSleep(sleepTimeSeconds * 1e6);
}
