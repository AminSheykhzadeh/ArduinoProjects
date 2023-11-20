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
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/4.4.0/chart.min.js" integrity="sha512-7U4rRB8aGAHGVad3u2jiC7GA5/1YhQcQjxKeaVms/bT66i3LVBMRcBI9KwABNWnxOSwulkuSXxZLGuyfvo7V1A==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    </head>

    <body onload= "javascript:init()">
     <div>
     <h1>hi</h1>
      <canvas id="line-chart" width="800" height="450">
      </canvas>
     </div>
     
     <!-- Adding a webSocket to the client (webpage)(js. code to get the data and display it on the page) -->
    <script>
      var webSocket, dataPlot;
      //<!--addData function-->
      function addData(lable, data){
          //push x val using dot notation
        dataPlot.data.lables.push(lable);
          //push x val using dot notation
        dataPlot.data.datasets[0].data.push(data);
          //call update methode so data ploted
        dataPlot.update();
      }
      //<!-- function init to run when page is loaded-->
      function init(){
        //<!--instance of WebSocket class running on port 81 -->
        webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
        //<!--define dataPlot variable to be an instance of chart class included-->
        dataPlot = new Chart(document.getElementById("line-chart"), {
          type: 'line',
          data: {
            lables: [],
            datasets: [{
              data:[],
              lable: "Counter"
              borderColor: "#3e95cd",
              fill: false
            }]
          }
        });
        //<!--onmessage methode to get event data from those measurements posted by server-->
        webSocket.onmessage = function(event){
          //<!--and parse the json formatted data to be displayed on page-->
          var data = JSON.parse(event.data);
          //<!--add data to the chart-->
            // X Axis: time stamp for when data sent to socket using Builtin Date class with today instance
          var today  = new Date();
            //use methodes to get hour min and sec in to string t
          var t = today.getHours() + ":" today.getMinutes() + ":" +  today.getSeconds();
            //func defined to get x and y value
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
  json += i++;
  json += "}";

  //broadcastTXT methode of web socket obj to sent that json string to the socket
  webSocket.broadcastTXT(json.c_str(),json.length());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  //do sth with the data from client
}