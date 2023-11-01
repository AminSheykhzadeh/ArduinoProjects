#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "NodeMCU_SoftAP";
const char *password = "password123";
unsigned long counter = 10000000;  // Counter starting at 10000000

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Set up NodeMCU as a SoftAP
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("SoftAP IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/getCounterValue", HTTP_GET, sendCounterValue);  // Endpoint to get counter value
  
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><head>";
  html += "<script type='text/javascript'>";
  html += "let storedData = localStorage.getItem('counterValues') ? JSON.parse(localStorage.getItem('counterValues')) : [];";
  html += "async function fetchCounterValue() {";
  html += "  const response = await fetch('/getCounterValue');";
  html += "  const counterValue = await response.text();";
  html += "  storedData.push(counterValue);";
  html += "  localStorage.setItem('counterValues', JSON.stringify(storedData));";
  html += "  displayData();";
  html += "}";
  html += "function displayData() {";
  html += "  document.getElementById('dataDisplay').innerHTML = storedData.join('<br>');";
  html += "}";
  html += "setInterval(fetchCounterValue, 10);"; // Call the function every 10ms
  html += "</script>";
  html += "</head><body onload='displayData()'>";
  html += "<h1>NodeMCU SoftAP</h1>";
  html += "<div id='dataDisplay'></div>"; // Display area for the logged data
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void sendCounterValue() {
  String counterValue = String(counter++);  // Increment the counter and convert to string
  server.send(200, "text/plain", counterValue);  // Send the counter value
}
