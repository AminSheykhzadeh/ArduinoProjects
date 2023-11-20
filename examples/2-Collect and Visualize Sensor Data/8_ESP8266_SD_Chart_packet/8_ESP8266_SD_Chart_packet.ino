#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <SD.h>
#include <FS.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
void handleRoot();
float readSensor1();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void logData(String timestamp, String location, String name, float temperature);
void handleExport();
float readSensor2();
float readSensor();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  if (!SD.begin(D8)) { // CS pin is connected to D8
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  server.on("/", HTTP_GET, handleRoot);
  server.on("/export", HTTP_GET, handleExport);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();

  String timestamp = String(millis()); // Example timestamp using millis
  String location = "32620272,51662376";           // Example location
  String name = "Ali Daei";            // Example name
  float temperature = readSensor();

  String jsonData = "{\"timestamp\":\"" + timestamp + "\", \"location\":\"" + location + "\", \"name\":\"" + name + "\", \"temperature\":" + String(temperature) + "}";
  
  logData(timestamp, location, name, temperature); // Log data to SD card
  webSocket.broadcastTXT(jsonData);

  delay(5000); // Send data every 5 seconds
}

void handleRoot() {
  String html = R"=====(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Sensor Dashboard</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
    <h3>Timestamp</h3>
    <canvas id="timestampChart" width="600" height="100"></canvas>
    
    <h3>Location</h3>
    <canvas id="locationChart" width="600" height="100"></canvas>

    <h3>Name</h3>
    <canvas id="nameChart" width="600" height="100"></canvas>

    <h3>Temperature</h3>
    <canvas id="temperatureChart" width="600" height="100"></canvas>

    <script>
            var data = {
                labels: [],
                datasets: [{
                    label: 'Sensor 1',
                    data: [],
                    borderColor: 'red',
                    fill: false
                },
                {
                    label: 'Sensor 2',
                    data: [],
                    borderColor: 'blue',
                    fill: false
                }]
            };

            var ctx = document.getElementById('sensorChart').getContext('2d');
            var chart = new Chart(ctx, {
                type: 'line',
                data: data
            });

        var ws = new WebSocket("ws://YOUR_NODEMCU_IP:81/");
        
        ws.onmessage = function(event) {
            var dataPacket = JSON.parse(event.data);

            // Update charts based on received data
            updateChart(timestampChart, dataPacket.timestamp);
            updateChart(locationChart, dataPacket.location);
            updateChart(nameChart, dataPacket.name);
            updateChart(temperatureChart, dataPacket.temperature);
        };

        function updateChart(chart, data) {
            if(chart.data.labels.length > 10) { // Limit data points for demonstration
                chart.data.labels.shift();
                chart.data.datasets[0].data.shift();
            }
            
            chart.data.labels.push(new Date().toLocaleTimeString());
            chart.data.datasets[0].data.push(data);
            
            chart.update();
        }

        var timestampChart = setupChart('timestampChart');
        var locationChart = setupChart('locationChart');
        var nameChart = setupChart('nameChart');
        var temperatureChart = setupChart('temperatureChart');

        function setupChart(elementId) {
            var ctx = document.getElementById(elementId).getContext('2d');
            return new Chart(ctx, {
                type: 'line',
                data: {
                    labels: [],
                    datasets: [{
                        data: [],
                        borderColor: 'blue',
                        fill: false
                    }]
                }
            });
        }
    </script>
    </body>
    </html>
    )=====";

    server.send(200, "text/html", html);
}

void handleExport() {
  File dataFile = SD.open("/data.csv", "r");
  if (!dataFile) {
    server.send(404, "text/plain", "Data file not found");
    return;
  }
  
  server.streamFile(dataFile, "text/csv");
  dataFile.close();
}

void logData(String timestamp, String location, String name, float temperature) {
  File dataFile = SD.open("/data.csv", "a");
  if (!dataFile) {
    Serial.println("Failed to open data file for writing");
    return;
  }
  
  String dataString = timestamp + "," + location + "," + name + "," + String(temperature) + "\n";
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

float readSensor() {
  // Your sensor's reading logic here
  return random(20, 30); // Dummy value for demonstration
}