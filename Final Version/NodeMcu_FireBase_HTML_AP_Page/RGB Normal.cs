#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
char* mySsid   = "AquaLink";
char* password = "123456789";
int pinArray[] = {14,12,13,15,0}; 
char webpage[] PROGMEM = R"=====(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>RGB Controller</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .sliderg { -webkit-appearance: none; margin: 1px; width: 360px; height: 10px; background: #66FF66;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .sliderg::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .sliderg::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
	
	    .sliderr { -webkit-appearance: none; margin: 1px; width: 360px; height: 10px; background: #ff3300;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .sliderr::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .sliderr::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }
	
	    .sliderb { -webkit-appearance: none; margin: 1px; width: 360px; height: 10px; background: #0033cc;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .sliderb::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .sliderb::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }
	
	    .sliderw { -webkit-appearance: none; margin: 1px; width: 360px; height: 10px; background: #f5f5f0;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .sliderw::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .sliderw::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; }
  </style>
</head>
<body>
  <h2>RGB Controller</h2>
  <p><span id="textWhite">White</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="white" min="0" max="1023"  step="1" class="sliderw"></p>
  <p><span id="textRed">Red</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="red" min="0" max="1023"  step="1" class="sliderr"></p>
  <p><span id="textGreen">Green</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="green" min="0" max="1023"  step="1" class="sliderg"></p>
  <p><span id="textBlue">Blue</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="blue" min="0" max="1023"  step="1" class="sliderb"></p>
<script>
function updateSliderPWM(element) {
  var cWhite = document.getElementById("white").value;
  var cRed = document.getElementById("red").value;
  var cGreen = document.getElementById("green").value;
  var cBlue = document.getElementById("blue").value;
  document.getElementById("textRed").innerHTML = cRed;
  document.getElementById("textGreen").innerHTML = cGreen;
  document.getElementById("textBlue").innerHTML = cBlue;
  document.getElementById("textWhite").innerHTML = cWhite;
  var data = {R:cRed, G:cGreen, B:cBlue , W:cWhite };
  var xhr = new XMLHttpRequest();
  xhr.open("POST", "/slidervalue", true);
  xhr.send(JSON.stringify(data));
}
</script>
</body>
</html>
)=====";


void setup()
{
 // Serial.begin(9600);
  pinMode(pinArray[0], OUTPUT);
  pinMode(pinArray[1], OUTPUT);
  pinMode(pinArray[2], OUTPUT);
  pinMode(pinArray[3], OUTPUT);
  pinMode(pinArray[4], OUTPUT);
      analogWrite(pinArray[0], 1023);
	  analogWrite(pinArray[1], 1023);
	  analogWrite(pinArray[2], 1023);
	  analogWrite(pinArray[3], 1023);
	  analogWrite(pinArray[4], 1023);
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();  
  server.close();        
  delay(1000);  
  WiFi.mode(WIFI_AP);
    WiFi.softAP(mySsid, password);
    IPAddress IP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  //  Serial.println(IP);
    delay(1000);
    server.on("/",[](){server.send_P(200,"text/html", webpage);});
    server.on("/slidervalue", HTTP_POST, getData);
    server.begin(); 
    delay(1000);  

}
void loop()
{  
 server.handleClient();
 //delay(200);
 
}
void getData()
{
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
      if(jObject.success())
      {
      int red  = jObject["R"];
	  int green  = jObject["G"];
	  int blue  = jObject["B"];
	  int white  = jObject["W"];
/*       Serial.println(String(red));
	  Serial.println(String(green));
	  Serial.println(String(blue));
	  Serial.println(String(white)); */
      //analogWriteFreq(2000);
      analogWrite(pinArray[0], red);
	  analogWrite(pinArray[1], green);
	  analogWrite(pinArray[2], blue);
	  analogWrite(pinArray[3], white);
	  analogWrite(pinArray[4], white);
      //analogWriteFreq(2000);
    }  
}
