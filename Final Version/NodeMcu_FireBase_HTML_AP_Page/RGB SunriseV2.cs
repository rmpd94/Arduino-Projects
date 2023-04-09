#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
char* mySsid   = "AquaLink";
char* password = "123456789";
int pinArray[] = {14, 12, 13, 15, 4};
unsigned long sMils = 0;
unsigned long pMils = 0;
//Mode 1- Manual, 2-Automatic
int dmode;
// Manual Intensities
int ir;
int ig;
int iw;
int ib;
//Sunrise Sunset Timings
int mr; //Max
int mg; //Max
int mb;//Max
int mw;//Max
int sr; //Starting
int sg; //Starting
int sb;//Starting
int sw;//Starting
int rmt;   //Sunrise time
int slt; //Sunset time
int tot; // Total Time


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
  <p><input type="range" onchange="updateSliderPWM(this)" id="white" min="0" max="255"  step="1" class="sliderw"></p>
  <p><span id="textRed">Red</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="red" min="0" max="255"  step="1" class="sliderr"></p>
  <p><span id="textGreen">Green</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="green" min="0" max="255"  step="1" class="sliderg"></p>
  <p><span id="textBlue">Blue</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="blue" min="0" max="255"  step="1" class="sliderb"></p>
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
  Serial.begin(9600);

  pinMode(pinArray[0], OUTPUT);
  pinMode(pinArray[1], OUTPUT);
  pinMode(pinArray[2], OUTPUT);
  pinMode(pinArray[3], OUTPUT);
  pinMode(pinArray[4], OUTPUT);
  SPIFFS.begin();
  syncStatus();
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
  server.on("/getStatus", HTTP_GET, getStatus);
  server.on("/saveModStngs", HTTP_POST, saveModStngs);
  server.on("/saveAutStngs", HTTP_POST, saveAutStngs);
  server.on("/saveManStngs", HTTP_POST, saveManStngs);
    server.begin(); 
    delay(1000);  

}
void loop()
{  
if (dmode == 2){
if (millis() - pMils > 2000 || pMils == 0) {
  pMils = millis();
  autoMode(pMils - sMils);
} 
}
if (dmode == 1){
  analogWrite(pinArray[0], ir);
    analogWrite(pinArray[1], ig);
    analogWrite(pinArray[2], ib);
    analogWrite(pinArray[3], iw);
    analogWrite(pinArray[4], iw);
}
server.handleClient();
}
void getData()
{
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
      if(jObject.success())
      {     
    int red  = jObject["R"];
    int green = jObject["G"];
    int blue  = jObject["B"];
    int white  = jObject["W"]; 

    analogWrite(pinArray[0], red);
    analogWrite(pinArray[1], green);
    analogWrite(pinArray[2], blue);
    analogWrite(pinArray[3], white);
    analogWrite(pinArray[4], white);
   ir  = red;
     ig = green;
     ib  = blue;
     iw  = white;
      //analogWriteFreq(2000);
    }  
}

/* Send Current Status */
void getStatus() {
 String data;
  data  = "{\"R\" : \"";
  data += ir;
  data += "\", ";
  data += "\"G\" : \"";
  data += ig;
  data += "\", ";
  data += "\"B\" : \""; 
  data += ib;
  data += "\", ";
  data += "\"W\" : \""; 
  data += iw;
  data += "\", ";
  data += "\"M\" : \""; 
  data += dmode;
  data += "\", ";
  data += "\"mr\" : \""; 
  data += mr;
  data += "\", ";
  data += "\"mg\" : \""; 
  data += mg;
  data += "\", ";
  data += "\"mb\" : \""; 
  data += mb; 
  data += "\", ";
  data += "\"mw\" : \""; 
  data += mw; 
  data += "\", ";
  data += "\"sr\" : \""; 
  data += sr; 
  data += "\", ";
  data += "\"sg\" : \""; 
  data += sg;   
  data += "\", ";
  data += "\"sb\" : \""; 
  data += sb;   
  data += "\", ";
  data += "\"sw\" : \""; 
  data += sw;
  data += "\", ";
  data += "\"rmt\" : \""; 
  data += rmt;   
  data += "\", ";
  data += "\"slt\" : \""; 
  data += slt;  
  data += "\", ";
  data += "\"tot\" : \""; 
  data += tot;
  data += "\"}";
  server.send(200, "text/html", data);      
}

void syncStatus()
{  
/*Check Mode from SPIFFS */
if(SPIFFS.exists("/modes.json")){
    
    File configFile = SPIFFS.open("/modes.json", "r");
    if(configFile){
    size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();
      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(buf.get());
          if(root.success())
      {
     String smode  = root["modes"];   
     dmode =  smode.toInt();
    }   
  } 
}else {
  Serial.println("else");
    dmode = 1; //Manual
    analogWrite(pinArray[0], 255);
    analogWrite(pinArray[1], 255);
    analogWrite(pinArray[2], 255);
    analogWrite(pinArray[3], 255);
    analogWrite(pinArray[4], 255);
  }
/*Check manual intensities from SPIFFS */
if(SPIFFS.exists("/mint.json")){
    
    File configFile = SPIFFS.open("/mint.json", "r");
    if(configFile){
    size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();

      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(buf.get());
          if(root.success())
      {
      String mint = root["mint"]; 
    setManVar(mint);
    }   
  } 
}
/*Check automatic setup from SPIFFS */
if(SPIFFS.exists("/atsetup.json")){
    
    File configFile = SPIFFS.open("/atsetup.json", "r");
    if(configFile){
    size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();

      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(buf.get());
          if(root.success())
      {
      String atsetup = root["atsetup"];  
    setAutVar(atsetup);
    }   
  }
}
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
//* Save Manual settings */
void saveManStngs()
{ 
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
    if(jObject.success())
      { File configFile = SPIFFS.open("/mint.json", "w");
    jObject.printTo(configFile);  
    configFile.close();
     String data  = jObject["mint"]; 
     
       setManVar (data);
       server.send(200, "text/html", "Manual Setup successfully Saved");
}
}

//* Save Automatic settings */
void saveAutStngs()
{ 
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
    if(jObject.success())
      { File configFile = SPIFFS.open("/atsetup.json", "w");
    jObject.printTo(configFile);  
    configFile.close();
     String data  = jObject["atsetup"]; 
       setAutVar(data);
	   sMils = millis();
       server.send(200, "text/html", "Auto Settings successfully Saved");
}
}
//* Save Modes settings */
void saveModStngs()
{ 
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
  File configFile = SPIFFS.open("/modes.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
    if(jObject.success())
      { 
        String data  = jObject["modes"]; 
        dmode =  data.toInt();
		if(dmode == 2){sMils = millis();}
       // Serial.println(data);
     server.send(200, "text/html", "Mode set successfully");
}
}

void setManVar(String input)
{ 
    ir = getValue(input,',', 0).toInt(); 
    ig = getValue(input,',', 1).toInt();
    ib = getValue(input,',', 2).toInt();
    iw = getValue(input,',', 3).toInt();

}
void setAutVar(String input)
{ 
     mr = getValue(input,',', 0).toInt();
     mg = getValue(input,',', 1).toInt();
     mb = getValue(input,',', 2).toInt();
     mw = getValue(input,',', 3).toInt();
     sr = getValue(input,',', 4).toInt();
     sg = getValue(input,',', 5).toInt();
     sb = getValue(input,',', 6).toInt();
     sw = getValue(input,',', 7).toInt();
     rmt = getValue(input,',', 8).toInt();
     slt = getValue(input,',', 9).toInt();
     tot = getValue(input,',', 10).toInt();
}
int geStr(String light,String type, long ntime)
{
 float vmax;
 float vmin;
 float vinterval;
 float returnVal;
 int lightval;
 //Serial.println(String(ntime));
 if (light == "R"){vmax =float(mr) ; 
                   vmin =float(sr);}
 if (light == "G"){vmax =float(mg) ;
                   vmin =float(sg); }
 if (light == "B"){vmax =float(mb) ;
                   vmin =float(sb);}
 if (light == "W"){vmax =float(mw) ;
                   vmin =float(sw);}
 vinterval = vmax - vmin;
 if (type == "R"){
 returnVal = slt;
 returnVal = vinterval/returnVal;
 returnVal = returnVal * float(ntime);
 lightval = round(vmax - returnVal);
 }
  if (type == "N"){
 returnVal = rmt;
 returnVal = vinterval/returnVal;
 returnVal = returnVal * float(ntime);
 lightval = round(returnVal + vmin );
 }

return lightval;

}   

void autoMode(long t) 
{

 long downt = (tot - slt) ;

/* Sunset */
  if (t >= downt ) {
  ir = geStr ("R","R",(t - downt));
  ig = geStr ("G","R",(t - downt));
  ib = geStr ("B","R",(t - downt));
  iw = geStr ("W","R",(t - downt));
    }
  
  /* Sun Rise */
   if (t < rmt ) {
  ir = geStr ("R","N",t);
  ig = geStr ("G","N",t);
  ib =  geStr ("B","N",t);
  iw =  geStr ("W","N",t);
    } 
    analogWrite(pinArray[0], ir);
    analogWrite(pinArray[1], ig);
    analogWrite(pinArray[2], ib);
    analogWrite(pinArray[3], iw);
    analogWrite(pinArray[4], iw);
}