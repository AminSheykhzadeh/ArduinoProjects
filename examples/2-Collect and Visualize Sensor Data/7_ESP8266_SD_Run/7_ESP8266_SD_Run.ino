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
void logData(float sensor1Value, float sensor2Value);
void handleExport();
float readSensor2();


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

  float sensor1Value = readSensor1();
  float sensor2Value = readSensor2();

  logData(sensor1Value, sensor2Value); // Log data to SD card

  String jsonData = "{\"sensor1\":" + String(sensor1Value) + ", \"sensor2\":" + String(sensor2Value) + "}";
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
        <canvas id="sensorChart" width="600" height="300"></canvas>

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

            var ws = new WebSocket("ws://)=====" + WiFi.localIP().toString() + R"=====(:81/");
            
            ws.onmessage = function(event) {
                var sensorsData = JSON.parse(event.data);
                
                if(data.labels.length > 10) { // Limit data points for demonstration
                    data.labels.shift();
                    data.datasets[0].data.shift();
                    data.datasets[1].data.shift();
                }
                
                data.labels.push(new Date().toLocaleTimeString());
                data.datasets[0].data.push(sensorsData.sensor1);
                data.datasets[1].data.push(sensorsData.sensor2);
                
                chart.update();
            };
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

void logData(float sensor1Value, float sensor2Value) {
  File dataFile = SD.open("/data.csv", "a");
  if (!dataFile) {
    Serial.println("Failed to open data file for writing");
    return;
  }
  
  String dataString = String(sensor1Value) + "," + String(sensor2Value) + "\n";
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
