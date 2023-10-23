#include <ESP8266WiFi.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";
WiFiServer server(80);
uint32_t hypotheticalPacket = 0xFFFFFF;  // 24-bit packet

unsigned long lastReportTime = 0;
unsigned long dataSent = 0;
unsigned long dataReceived = 0;

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
    // Send hypothetical packet to client
    client.write((hypotheticalPacket >> 16) & 0xFF);
    client.write((hypotheticalPacket >> 8) & 0xFF);
    client.write(hypotheticalPacket & 0xFF);
    dataSent += 24;

    // Non-blocking wait for the client to send the packet back
    unsigned long waitStartTime = millis();
    while(client.available() < 3) {
      yield();  // Attend to background tasks
      if (millis() - waitStartTime > 1000) {  // Timeout after 1 second
        Serial.println("Client response timeout!");
        client.stop();
        return;
      }
    }
    
    uint32_t receivedPacket = client.read() << 16 |
                              client.read() << 8  |
                              client.read();
    dataReceived += 24;

    client.stop();

    if (millis() - lastReportTime >= 1000) {
      float uploadSpeed = dataSent;  // bits per second (bps)
      float downloadSpeed = dataReceived;  // bits per second (bps)

      Serial.printf("Upload Speed: %.2f bps\n", uploadSpeed);
      Serial.printf("Download Speed: %.2f bps\n", downloadSpeed);

      lastReportTime = millis();
      dataSent = 0;
      dataReceived = 0;
    }
  }
}
