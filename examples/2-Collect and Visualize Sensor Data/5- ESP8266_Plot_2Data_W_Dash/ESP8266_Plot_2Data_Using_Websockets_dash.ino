#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "a";
const char* password = "aaaaaaaa";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
float readSensor1();
float readSensor2();
void handleRoot();

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

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();

  float sensor1Value = readSensor1();
  float sensor2Value = readSensor2();

  String jsonData = "{\"sensor1\":" + String(sensor1Value) + ", \"sensor2\":" + String(sensor2Value) + "}";
  webSocket.broadcastTXT(jsonData);

  delay(100); // Send data every 0.5 seconds

    Serial.println(jsonData);

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
                
                if(data.labels.length > 5) { // Limit data points for demonstration
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

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // Handle WebSocket events here if needed
}

float readSensor1() {
  // Your sensor1's reading logic here
  return random(20, 30); // Dummy value for demonstration
}

float readSensor2() {
  // Your sensor2's reading logic here
  return random(50, 70); // Dummy value for demonstration
}