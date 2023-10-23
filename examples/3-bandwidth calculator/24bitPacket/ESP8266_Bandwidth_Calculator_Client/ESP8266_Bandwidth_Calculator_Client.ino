// const char* ssid = "a";
// const char* password = "aaaaaaaa";
// const char* serverIP = "192.168.43.43";
#include <ESP8266WiFi.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";
const char* serverIP = "192.168.43.43";

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
  Serial.println("Connecting to server...");
  WiFiClient client;
  if (client.connect(serverIP, 80)) {
    uint32_t receivedPacket = client.read() << 16 |
                              client.read() << 8  |
                              client.read();

    client.write((receivedPacket >> 16) & 0xFF);
    client.write((receivedPacket >> 8) & 0xFF);
    client.write(receivedPacket & 0xFF);

    client.stop();
    Serial.println("Packet received and sent back!");
  } else {
    Serial.println("Failed to connect to server.");
  }
  delay(100);  // Small delay before reconnecting
}
