#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>

const char *ssid = "NodeMCU_SoftAP";
const char *password = "password123";
String counter = "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

ESP8266WebServer server(80);

// DNS Server setup
const byte DNS_PORT = 53;
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);

void setup() {
  Serial.begin(115200);

  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.hostname("nodemcu");

  // Set up NodeMCU as a SoftAP
  WiFi.softAP(ssid, password);
  Serial.println();
  Serial.print("SoftAP IP address: ");
  Serial.println(WiFi.softAPIP());

  // Start the DNS server
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/getCounterValue", HTTP_GET, sendCounterValue);
  
  server.begin();
}

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();  // Process DNS requests
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
  html += "  let formattedData = storedData.map((value, index) => `${index + 1}. ${value}`);"; // Add line numbers
  html += "  document.getElementById('dataDisplay').innerHTML = formattedData.join('<br>');";
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
