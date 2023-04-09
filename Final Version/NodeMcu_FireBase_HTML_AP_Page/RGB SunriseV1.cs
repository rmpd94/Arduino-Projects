#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
char* mySsid   = "AquaLink";
char* password = "123456789";
int mr = 100; //Max
int mg = 100; //Max
int mb = 100;//Max
int smr = 50; //min
int smg = 0; //min
int smb = 0;//min
int sunrise = 1;  
int sunset  = 1;
int total   = 3;
unsigned long pMils = 0;

char webpage[] PROGMEM = R"=====(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP Web Server</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
  </style>
</head>
<body>
  <h2>RGB Controller</h2>
  <p><span id="textRed">Red</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="red" min="0" max="255"  step="1" class="slider"></p>
  <p><span id="textGreen">Green</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="green" min="0" max="255"  step="1" class="slider"></p>
  <p><span id="textBlue">Blue</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="blue" min="0" max="255"  step="1" class="slider"></p>
<script>
function updateSliderPWM(element) {
  var cRed = document.getElementById("red").value;
  var cGreen = document.getElementById("green").value;
  var cBlue = document.getElementById("blue").value;
  document.getElementById("textRed").innerHTML = cRed;
  document.getElementById("textGreen").innerHTML = cGreen;
  document.getElementById("textBlue").innerHTML = cBlue;
  var data = {R:cRed, G:cGreen, B:cBlue };
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
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();  
  server.close();        
  delay(1000);  
  WiFi.mode(WIFI_AP);
    WiFi.softAP(mySsid, password);
    IPAddress IP = WiFi.softAPIP();
   Serial.print("AP IP address: ");
    Serial.println(IP);
    delay(1000);
    server.on("/",[](){server.send_P(200,"text/html", webpage);});
    server.on("/slidervalue", HTTP_POST, getData);
    server.begin(); 
    delay(1000);  

}
void loop()
{ 

if (millis() - pMils > 2000 || pMils == 0) {
  pMils = millis();
handleOffSch(pMils);
server.handleClient();
}


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
      Serial.println(String(red));
    Serial.println(String(green));
    Serial.println(String(blue));
      //analogWriteFreq(2000);
      analogWrite(2, red);
    analogWrite(14, green);
    analogWrite(12, blue);
      analogWriteFreq(2000);
    }  
}


int geStr(String light,String type, long ntime)
{
 float vmax;
 float vmin;
 float vinterval;
 float returnVal;
 int lightval;
 Serial.println(String(ntime));
 if (light == "R"){vmax =float(mr) ; 
                   vmin =float(smr);}
 if (light == "G"){vmax =float(mg) ;
                   vmin =float(smg); }
 if (light == "B"){vmax =float(mb) ;
                   vmin =float(smb);}
 vinterval = vmax - vmin;
 returnVal = sunrise * 60000.00;
 returnVal = vinterval/returnVal;
 returnVal = returnVal * float(ntime);

 if (type == "R"){
 lightval = round(vmax - returnVal);
 }
  if (type == "N"){
 lightval = round(returnVal + vmin );
 }

return lightval;

}   


void handleOffSch(long t) 
{
  int red;
  int green;
  int blue;
  long downt;
  red = 0;
  green =0;
  blue =0;
  downt = (total - sunset) * 60000;

/* SunSet */
  if (t >= downt ) {
  red = geStr ("R","R",(t - downt));
  green = geStr ("G","R",(t - downt));
  blue =  geStr ("B","R",(t - downt));
  analogWrite(2, red);
  analogWrite(14, green);
  analogWrite(12,blue);
    }
	
	/* Sun Rise */
   if (t < sunrise*60000 ) {
  red = geStr ("R","N",t);
  green = geStr ("G","N",t);
  blue =  geStr ("B","N",t);
  analogWrite(2, red);
  analogWrite(14, green);
  analogWrite(12,blue);
    }
   
  //Serial.println(String(t));
  //Serial.println(String(red));
  //Serial.println(String(green));
  //Serial.println(String(blue));
  //Serial.println("--------");
}