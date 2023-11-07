#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "A";
const char* password = "aaaaaaaa";

ESP8266WebServer server(80);

String webPage;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  float temperature = readTemperature();

  webPage = "<html><head><script src='https://cdn.jsdelivr.net/npm/chart.js'></script></head><body>";
  webPage += "<h1>Temperature: " + String(temperature) + " C</h1>";
  webPage += "<canvas id='temperatureChart' width='400' height='200'></canvas>";
  webPage += "<script>var ctx = document.getElementById('temperatureChart').getContext('2d');";
  webPage += "var chart = new Chart(ctx, {type: 'line', data: {labels: [1], datasets: [{label: 'Temperature', data: [" + String(temperature) + "], borderColor: 'red', fill: false}]}});";
  webPage += "</script></body></html>";

  server.send(200, "text/html", webPage);
}

float readTemperature() {
  // Your sensor's reading logic here
  return random(20, 30); // This is a dummy value for demonstration purposes
}