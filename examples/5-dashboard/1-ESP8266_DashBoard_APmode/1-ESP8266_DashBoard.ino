#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);

    // Initialize SPIFFS
    if (!SPIFFS.begin()) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    // Start the WiFi access point
    WiFi.softAP("NodeMCU", "password123");
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // Serve the HTML page
    server.on("/", HTTP_GET, []() {
        File file = SPIFFS.open("/index.html", "r");
        server.streamFile(file, "text/html");
        file.close();
    });

    // Serve the CSS file
    server.on("/styles.css", HTTP_GET, []() {
        File file = SPIFFS.open("/style.css", "r");
        server.streamFile(file, "text/css");
        file.close();
    });

    // Serve the CSS file
    server.on("/index.js", HTTP_GET, []() {
        File file = SPIFFS.open("/index.js", "r");
        server.streamFile(file, "text/js");
        file.close();
    });

    // Serve the CSS file
    server.on("/orders.js", HTTP_GET, []() {
        File file = SPIFFS.open("/orders.js", "r");
        server.streamFile(file, "text/js");
        file.close();
    });

    server.begin();
}

void loop() {
    server.handleClient();
}
