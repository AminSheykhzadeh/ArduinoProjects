//Using NodeMCU Running on ESP8266
//Code Learned From https://www.youtube.com/watch?v=lEVoRJSsEa8&t=1s&pp=ygUxRVNQODI2Nl9QbG90X1NlbnNvcl9EYXRhX1dpdGhfV2Vic29ja2V0c19DaGFydF9qcw%3D%3D
//Author : Amin.Sheykhzadeh.ir

#include <Arduino.h>
#include <ESP8266WiFi.h>          //connect wifi
#include <ESP8266WebServer.h>     //start server
#include <WebSocketsServer.h>      //live communicate
#include <Ticker.h>               //timer

unsigned int i=0;
// collect sensor data
Ticker timer;                     //object of Ticker class

//Setup web server, connect to internet
char * ssid = "EPC-GAME1";
char * password = "";

// char * ssid = "KianPirFalak";
// char * password = "123456710";

// Running a webserver server instance of ESP8266WebServer class
ESP8266WebServer server(80);

// Add websockets to the server
WebSocketsServer webSocket = WebSocketsServer(81);

//serving a Webpage
                          // using string literal notation  (from flash memory)
char webpage[] PROGMEM = R"=====(
  <html>
    <head>
    <script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js'></script>
    </head>

    <body onload= "javascript:init()">
     <div>
      <h1>hi</h1>
      <canvas id="line-chart" width="800" height="450">
      </canvas>
     </div>
     
    <script>
      var webSocket, dataPlot;
      function addData(label, data){
        dataPlot.data.labels.push(label);
        dataPlot.data.datasets[0].data.push(data);
        dataPlot.update();
      }
      function init(){
        webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
        dataPlot = new Chart(document.getElementById("line-chart"), {
          type: 'line',
          data: {
            labels: [],
            datasets: [{
              data:[],
              label: "Counter",
              borderColor: "#3e95cd",
              fill: false
            }]
          }
        });
        webSocket.onmessage = function(event){
          var data = JSON.parse(event.data);
          var today  = new Date();
          var t = today.getHours() + ":" + today.getMinutes() + ":" +  today.getSeconds();
          addData(t , data.value);
        }
      }
    </script>
   </body>
  </html>
)=====";

void setup () {
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(". ");
    delay(500);
  }

  Serial.println("");Serial.println("");
  Serial.println("");Serial.println("");
  Serial.println("--------IP:--------");
  Serial.println(WiFi.localIP());//show IP taken

  //route to webpage when a request to root path, call an anonymous function to send page
  server.on("/",[](){
    server.send_P(200, "text/html", webpage );
    // server.send(200, "text/plain", "Hello world!");
  });

  
  //begin methode of server
  server.begin();
  //begin methode of websocket object
  webSocket.begin();
  // on event methode that will call a function any time a data is sent from client to the server over websocket
  webSocket.onEvent(webSocketEvent);
  //every 1 sec trigger getData funtion
  timer.attach(1, getData); 
}//setup

void loop() {
  //loop methode of webSocket object 
  webSocket.loop();
  //handle client methode in loop function
  server.handleClient();
}

void getData() {
  //Serial.println(random());
  String json = "{\"value\":";
  //json += i+ random(10,20);;
  json += random(10,100);
  json += "}";

  //broadcastTXT methode of web socket obj to sent that json string to the socket
  webSocket.broadcastTXT(json.c_str(),json.length());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  //do sth with the data from client
}