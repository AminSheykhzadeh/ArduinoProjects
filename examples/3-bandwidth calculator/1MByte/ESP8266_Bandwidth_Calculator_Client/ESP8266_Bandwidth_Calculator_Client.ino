// const char* ssid = "a";
// const char* password = "aaaaaaaa";
// const char* serverIP = "192.168.43.43";
#include <ESP8266WiFi.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";
const char* serverIP = "192.168.43.43";

const int PACKET_SIZE = 512;
const int TOTAL_CHUNKS = 2048;  // For 1 MByte of data

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  WiFiClient client;
  if (client.connect(serverIP, 80)) {
    // Request data
    client.write('R');

    // Receive data
    for (int i = 0; i < TOTAL_CHUNKS; i++) {
      for (int j = 0; j < PACKET_SIZE; j++) {
        while (!client.available()) yield();
        client.read();
      }
    }

    // Acknowledge reception
    client.write('A');

    // Send data back
    for (int i = 0; i < TOTAL_CHUNKS; i++) {
      for (int j = 0; j < PACKET_SIZE; j++) {
        client.write(0xFF);
      }
    }
    client.stop();
    Serial.println("Data exchange completed.");
    delay(2000);  // Wait 2 seconds before next loop
  } else {
    Serial.println("Failed to connect to server.");
    delay(5000);  // If failed, wait longer before retrying
  }
}
