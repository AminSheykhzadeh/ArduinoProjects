#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "NodeMCU_SoftAP";
const char *password = "password123";
String counter = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

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
  html += "  storedData.unshift(counterValue);"; // Add new value to the beginning
  html += "  localStorage.setItem('counterValues', JSON.stringify(storedData));";
  html += "  displayData();";
  html += "  requestAnimationFrame(fetchCounterValue);"; // Continue requesting with max speed
  html += "}";
  html += "function displayData() {";
  html += "  document.getElementById('dataDisplay').innerHTML = storedData.join('<br>');";
  html += "}";
  html += "window.onload = function() {";
  html += "  displayData();";
  html += "  requestAnimationFrame(fetchCounterValue);"; // Start requesting with max speed
  html += "}";
  html += "</script>";
  html += "</head><body>";
  html += "<h1>NodeMCU SoftAP</h1>";
  html += "<div id='dataDisplay'></div>"; // Display area for the logged data
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void incrementCounter() {
  // Custom increment function for the 100-digit counter
  int length = counter.length();
  for (int i = length - 1; i >= 0; i--) {
    if (counter[i] != '9') {
      counter[i]++;
      break;
    } else {
      counter[i] = '0';
    }
  }
}

void sendCounterValue() {
  incrementCounter();  // Increment the counter
  server.send(200, "text/plain", counter.c_str());  // Send the counter value
}
