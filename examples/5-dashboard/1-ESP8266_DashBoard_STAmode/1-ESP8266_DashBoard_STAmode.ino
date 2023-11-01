#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const char *ssid = "a";
const char *password = "aaaaaaaa";

ESP8266WebServer server(80);

void setup() {
    Serial.begin(115200);

    if (!SPIFFS.begin()) {
        Serial.println("An error has occurred while mounting SPIFFS");
        return;
    }

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.println("IP Address: ");
    Serial.println(WiFi.localIP());

    // Define endpoints
    server.on("/", HTTP_GET, []() {
        File file = SPIFFS.open("/index.html", "r");
        server.streamFile(file, "text/html");
        file.close();
    });

    server.on("/style.css", HTTP_GET, []() {
        File file = SPIFFS.open("/style.css", "r");
        server.streamFile(file, "text/css");
        file.close();
    });

    server.on("/index.js", HTTP_GET, []() {
        File file = SPIFFS.open("/index.js", "r");
        server.streamFile(file, "text/js");
        file.close();
    });

    server.on("/orders.js", HTTP_GET, []() {
        File file = SPIFFS.open("/orders.js", "r");
        server.streamFile(file, "text/js");
        file.close();
    });

    server.on("/Admin.jpg", HTTP_GET, []() {
        File file = SPIFFS.open("/Admin.jpg", "r");
        server.streamFile(file, "image/jpeg");
        file.close();
    });

    server.on("/profile-1.jpg", HTTP_GET, []() {
        File file = SPIFFS.open("/profile-1.jpg", "r");
        server.streamFile(file, "image/jpeg");
        file.close();
    });

    server.on("/profile-2.jpg", HTTP_GET, []() {
        File file = SPIFFS.open("/profile-2.jpg", "r");
        server.streamFile(file, "image/jpeg");
        file.close();
    });

    server.on("/profile-3.jpg", HTTP_GET, []() {
        File file = SPIFFS.open("/profile-3.jpg", "r");
        server.streamFile(file, "image/jpeg");
        file.close();
    });

    server.on("/logo.png", HTTP_GET, []() {
        File file = SPIFFS.open("/logo.png", "r");
        server.streamFile(file, "image/png");
        file.close();
    });

    server.on("/plus.png", HTTP_GET, []() {
        File file = SPIFFS.open("/plus.png", "r");
        server.streamFile(file, "image/png");
        file.close();
    });

    server.begin();
}

void loop() {
    server.handleClient();
}
