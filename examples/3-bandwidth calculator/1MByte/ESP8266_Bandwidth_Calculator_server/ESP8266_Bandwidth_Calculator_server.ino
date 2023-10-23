#include <ESP8266WiFi.h>
const char* ssid = "a";
const char* password = "aaaaaaaa";
const char* serverIP = "192.168.43.43";
WiFiServer server(80);

const int PACKET_SIZE = 512;
const int TOTAL_CHUNKS = 2048;  // For 1 MByte of data
unsigned long dataSent = 0;
unsigned long dataReceived = 0;
unsigned long lastReportTime = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  server.begin();
  Serial.println("Server started and waiting for client...");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    // Wait for client to request data
    while (!client.available()) yield();
    if (client.read() == 'R') {
      // Send the data
      for (int i = 0; i < TOTAL_CHUNKS; i++) {
        for (int j = 0; j < PACKET_SIZE; j++) {
          client.write(0xFF);
        }
        dataSent += PACKET_SIZE * 8;
      }
    }

    // Wait for client acknowledgment
    while (!client.available()) yield();
    if (client.read() == 'A') {
      // Receive the data back
      for (int i = 0; i < TOTAL_CHUNKS; i++) {
        for (int j = 0; j < PACKET_SIZE; j++) {
          while (!client.available()) yield();
          client.read();
        }
        dataReceived += PACKET_SIZE * 8;
      }
    }
    client.stop();

    if (millis() - lastReportTime >= 10000) {
      float uploadSpeed = dataSent / 10.0;  // bps
      float downloadSpeed = dataReceived / 10.0;  // bps

      Serial.printf("Upload Speed: %.2f bps\n", uploadSpeed);
      Serial.printf("Download Speed: %.2f bps\n", downloadSpeed);

      lastReportTime = millis();
      dataSent = 0;
      dataReceived = 0;
    }
  }
}
