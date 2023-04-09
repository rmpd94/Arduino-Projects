#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <RtcDS3231.h>

const char* ssid     = "ESP8266 Remote Control";
const char* password = "Paradise";
WiFiServer server(80);
ESP8266WebServer server1(90);
String header;
RtcDS3231<TwoWire> Rtc(Wire);
const int output16 = 16; // for D0 of Node MCU
//const int output5 =   5; // for D1 of Node MCU  -- connect SDA to D2 (GPIO 4)
                                                  //         SCL to D1 (GPIO 5) in RTC Module
//const int output4 =   4; // for D2 of Node MCU
const int output0 =   0; // for D3 of Node MCU
const int output2 =   2; // for D4 of Node MCU
const int output14 = 14; // for D5 of Node MCU
const int output12 = 12; // for D6 of Node MCU
const int output13 = 13; // for D7 of Node MCU
const int output15 = 15; // for D8 of Node MCU
int pinArray[] = {14,12,13,15};
int S1Si ;
String NPMH;
String NPMM;
String NPMD;
String S1UNIQUE;
String S2UNIQUE;
String S3UNIQUE;
String S4UNIQUE;
String S5UNIQUE;
String S6UNIQUE;
String S7UNIQUE;
String S8UNIQUE;
String NTPUNIQUE;
String SCHEDULE1;
String SCHEDULE2;
String SCHEDULE3;
String SCHEDULE4;
String SCHEDULE5;
String SCHEDULE6;
String SCHEDULE7;
String SCHEDULE8;

char webpage[] PROGMEM = R"=====(
<html>
<head>
</head>
<style>
body {background-color: powderblue;}
h1 {text-align: center;} 
label   {color: black; margin: 2px auto;}
input    {color: blue; margin: 2px auto;}
button {margin: 5px auto;
        display:block;}
form {
  width: 300px;
  margin: 50px auto;
}
</style>
<body>
<h1>Aqua Link Schedule Console</h1>
  <form>
    <fieldset>
      <div>
        <label for="schedule1">Schedule 1 :</label>      
        <input value="" id="schedule1" placeholder="Schedule1">
      </div>
          <div>
        <label for="schedule2">Schedule 2 :</label>      
        <input value="" id="schedule2" placeholder="Schedule2">
      </div>
          <div>
        <label for="schedule3">Schedule 3 :</label>      
        <input value="" id="schedule3" placeholder="Schedule3">
      </div>
          <div>
        <label for="schedule4">Schedule 4 :</label>      
        <input value="" id="schedule4" placeholder="Schedule4">
      </div>
          <div>
        <label for="schedule5">Schedule 5 :</label>      
        <input value="" id="schedule5" placeholder="Schedule5">
      </div>
          <div>
        <label for="schedule6">Schedule 6 :</label>      
        <input value="" id="schedule6" placeholder="Schedule6">
      </div>
          <div>
        <label for="schedule7">Schedule 7 :</label>      
        <input value="" id="schedule7" placeholder="Schedule7">
      </div>
          <div>
        <label for="schedule8">Schedule 8 :</label>      
        <input value="" id="schedule8" placeholder="Schedule8">
      </div>
      <div>
        <button class="primary" id="savebtn" type="button" onclick="myFunction()">Save</button>
      </div>
    </fieldset>
  </form>
</body>
<script>
function myFunction()
{
  console.log("button was clicked!");
var schedule1 = document.getElementById("schedule1").value;
var schedule2 = document.getElementById("schedule2").value;
var schedule3 = document.getElementById("schedule3").value;
var schedule4 = document.getElementById("schedule4").value;
var schedule5 = document.getElementById("schedule5").value;
var schedule6 = document.getElementById("schedule6").value;
var schedule7 = document.getElementById("schedule7").value;
var schedule8 = document.getElementById("schedule8").value;
  var data = {schedule1:schedule1,schedule2:schedule2,schedule3:schedule3,schedule4:schedule4,schedule5:schedule5,schedule6:schedule6,schedule7:schedule7,schedule8:schedule8};
  var xhr = new XMLHttpRequest();
  var url = "/set/settings";
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      // Typical action to be performed when the document is ready:
      if(xhr.responseText != null){
        console.log(xhr.responseText);
      }
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(data));
};
</script>
</html>
)=====";
void setup() {
  Serial.begin(115200);
  pinMode(output16, OUTPUT);
 // pinMode(output5, OUTPUT);
 // pinMode(output4, OUTPUT);
  pinMode(output0, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output14, OUTPUT);
  pinMode(output12, OUTPUT);
  pinMode(output13, OUTPUT);
  pinMode(output15, OUTPUT); 
  digitalWrite(output16, LOW);
 // digitalWrite(output5, LOW);
 // digitalWrite(output4, LOW);
  digitalWrite(output0, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(output14, LOW);
  digitalWrite(output12, LOW);
  digitalWrite(output13, LOW);
  digitalWrite(output15, LOW);
  SPIFFS.begin();
  Rtc.Begin();
  // Rtc.SetDateTime(RtcDateTime("Jul 05 2020", "00:42:00")); // Aprox 45 seconds to upload the code. use this to set time.. then comment and re upload
      
     // Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__)); //to set date from system
   // Rtc.Enable32kHzPin(false);
   // Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server1.on("/set/",[](){server1.send_P(200,"text/html", webpage);});
  server1.on("/set/settings", HTTP_POST, handleSettingsUpdate);
server1.begin();
 server.begin();
  
}

void loop(){
 wifiConnect();
 server1.handleClient();
 doManualTask ();
 
}

void doManualTask () {
   WiFiClient client = server.available();   

  if (client) {                            
    String currentLine = "";               
    while (client.connected()) {           
      if (client.available()) {             
        char c = client.read();            
        header += c;
        if (c == '\n') {                   
          if (currentLine.length() == 0) {

       if (header.indexOf("GET /16/on") >= 0)
            {
              digitalWrite(output16, LOW);
              break;
            } 
            else if (header.indexOf("GET /16/off") >= 0)
            {
              digitalWrite(output16, HIGH);
              break;
            } 
      
 /*     else if (header.indexOf("GET /5/on") >= 0)
            {
              digitalWrite(output5, LOW);
              break;
            } 
            else if (header.indexOf("GET /5/off") >= 0)
            {
              digitalWrite(output5, HIGH);
              break;
            } 
      else if (header.indexOf("GET /4/on") >= 0)
            {
              digitalWrite(output4, LOW);
              break;
            } 
            else if (header.indexOf("GET /4/off") >= 0)
            {
              digitalWrite(output4, HIGH);
              break;
            }  */
      else if (header.indexOf("GET /0/on") >= 0)
            {
              digitalWrite(output0, LOW);
              break;
            } 
            else if (header.indexOf("GET /0/off") >= 0)
            {
              digitalWrite(output0, HIGH);
              break;
            } 

      else if (header.indexOf("GET /2/on") >= 0)
            {
              digitalWrite(output2, LOW);
              break;
            } 
            else if (header.indexOf("GET /2/off") >= 0)
            {
              digitalWrite(output2, HIGH);
              break;
            } 
             else if (header.indexOf("GET /14/on") >= 0)
            {
              digitalWrite(output14, LOW);
              break;
            } 
            else if (header.indexOf("GET /14/off") >= 0)
            {
              digitalWrite(output14, HIGH);
              break;
            }     
            if (header.indexOf("GET /12/on") >= 0) 
            {
              digitalWrite(output12, LOW);
              break;
            } 
            else if (header.indexOf("GET /12/off") >= 0) 
            {
              digitalWrite(output12, HIGH);
              break;
            }
            else if (header.indexOf("GET /13/on") >= 0)
            {
              digitalWrite(output13, LOW);
              break;
            } 
            else if (header.indexOf("GET /13/off") >= 0)
            {
              digitalWrite(output13, HIGH);
              break;
            }            
            else if (header.indexOf("GET /15/on") >= 0)
            {
              digitalWrite(output15, LOW);
              break;
            } 
            else if (header.indexOf("GET /15/off") >= 0)
            {
              digitalWrite(output15, HIGH);
              break;
            }        
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;     
        }
      }
    }
    header = "";
    client.stop();
  }
}
void handleSettingsUpdate()
{
  String data = server1.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);

  File configFile = SPIFFS.open("/config.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
  
  server1.send(200, "application/json", "{\"status\" : \"ok\"}");
  delay(2000);
  
}
void wifiConnect()
{
  if(SPIFFS.exists("/config.json")){
    const char *_schedule1 = "", *_schedule2 = "", *_schedule3 = "", *_schedule4 = "", *_schedule5 = "", *_schedule6 = "", *_schedule7 = "", *_schedule8 = "";
    File configFile = SPIFFS.open("/config.json", "r");
    if(configFile){
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();

      DynamicJsonBuffer jsonBuffer;
      JsonObject& jObject = jsonBuffer.parseObject(buf.get());
      if(jObject.success())
      {
         _schedule1 = jObject["schedule1"];
         _schedule2 = jObject["schedule2"];
         _schedule3 = jObject["schedule3"];
         _schedule4 = jObject["schedule4"];
         _schedule5 = jObject["schedule5"];
         _schedule6 = jObject["schedule6"];
         _schedule7 = jObject["schedule7"];
         _schedule8 = jObject["schedule8"]; 
         SCHEDULE1 = _schedule1; 
         SCHEDULE2 = _schedule2;
         SCHEDULE3 = _schedule3;
         SCHEDULE4 = _schedule4;
         SCHEDULE5 = _schedule5;
         SCHEDULE6 = _schedule6;
         SCHEDULE7 = _schedule7; 
         SCHEDULE8 = _schedule8;
    RtcDateTime now = Rtc.GetDateTime();
  
    NPMH = now.Hour()  ;
    NPMM =  now.Minute() ;
    NPMD =  now.Day() ;
  if (NPMH.length() < 2 ) {NPMH = '0' + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = '0' + NPMM; }
    
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;
      
    if (NPMH == SCHEDULE1.substring(4,6) && NPMM == SCHEDULE1.substring(7,9) && NTPUNIQUE != S1UNIQUE  ){
    S1Si = SCHEDULE1.substring(0,1).toInt();
    if (SCHEDULE1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S1UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S1UNIQUE = NTPUNIQUE;
      loop();
    }
  }

    if (NPMH == SCHEDULE2.substring(4,6) && NPMM == SCHEDULE2.substring(7,9) && NTPUNIQUE != S2UNIQUE  ){
    S1Si = SCHEDULE2.substring(0,1).toInt();
    if (SCHEDULE2.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S2UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE2.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S2UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  
      if (NPMH == SCHEDULE3.substring(4,6) && NPMM == SCHEDULE3.substring(7,9) && NTPUNIQUE != S3UNIQUE  ){
    S1Si = SCHEDULE3.substring(0,1).toInt();
    if (SCHEDULE3.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S3UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE3.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S3UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  
      if (NPMH == SCHEDULE4.substring(4,6) && NPMM == SCHEDULE4.substring(7,9) && NTPUNIQUE != S4UNIQUE  ){
    S1Si = SCHEDULE4.substring(0,1).toInt();
    if (SCHEDULE4.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S4UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE4.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S4UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  
      if (NPMH == SCHEDULE5.substring(4,6) && NPMM == SCHEDULE5.substring(7,9) && NTPUNIQUE != S5UNIQUE  ){
    S1Si = SCHEDULE5.substring(0,1).toInt();
    if (SCHEDULE5.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S5UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE5.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S5UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  
      if (NPMH == SCHEDULE6.substring(4,6) && NPMM == SCHEDULE6.substring(7,9) && NTPUNIQUE != S6UNIQUE  ){
    S1Si = SCHEDULE6.substring(0,1).toInt();
    if (SCHEDULE6.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S6UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE6.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S6UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  
      if (NPMH == SCHEDULE7.substring(4,6) && NPMM == SCHEDULE7.substring(7,9) && NTPUNIQUE != S7UNIQUE  ){
    S1Si = SCHEDULE7.substring(0,1).toInt();
    if (SCHEDULE7.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S7UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE7.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S7UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  
      if (NPMH == SCHEDULE8.substring(4,6) && NPMM == SCHEDULE8.substring(7,9) && NTPUNIQUE != S8UNIQUE  ){
    S1Si = SCHEDULE8.substring(0,1).toInt();
    if (SCHEDULE8.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      S8UNIQUE = NTPUNIQUE;
    loop();
    } else if (SCHEDULE8.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      S8UNIQUE = NTPUNIQUE;
      loop();
    }
  }
  




  
    } 
  }
}


}

