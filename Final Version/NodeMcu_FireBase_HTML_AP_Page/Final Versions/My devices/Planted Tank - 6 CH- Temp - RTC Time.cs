#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <time.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define user "/AquaPlant"
#define firebasePath "/AquaPlant/powers/P_"
#define  ONE_WIRE_BUS 0            //D3 pin of nodemcu

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ESP8266WebServer server;
char* mySsid   = "AquaLink";
char* password = "123456789";
RtcDS3231<TwoWire> Rtc(Wire);
FirebaseData firebaseData;
// SDA --> D2 (GPIO 4)
// SCL --> D1 (GPIO 5)  RTC
const String add0 = "0";
int pinArray[] = {16, 2, 14, 12, 13, 15}; //D0,D4,D5,D6,D7,D8

String SCHMUNIQUE;
String SNDPWSTAT;
boolean internet;



char webpage[] PROGMEM = R"=====(
<html>
<head>
</head>
<style>
body {background-color: powderblue;}
h1 {text-align: center;} 
label   {display: inline-block; color: black; margin: 2px auto; width:100px; }
input    {display: inline-block; color: blue; margin: 2px auto;}
button {margin: 5px auto;
        display:block;}
form {
  width: 300px;
  margin: 50px auto;
}
</style>
<body>
<h1>Aqua Link Online Device Configuration Page</h1>
  <form>
    <fieldset>
      <div>
        <label for="ssid">WiFi Name</label>      
        <input value="" id="ssid" placeholder="Wifi SSID">
      </div>
      <div>
        <label for="password">WiFi Password</label>
        <input value="" id="password" placeholder="Wifi Password">
      </div>
    <div>
        <label for="firebaseHost">Firebase Host</label>
        <input value="" id="firebaseHost" placeholder="Firebase Host">
      </div>
    <div>
        <label for="firebaseAuth">Firebase Auth</label>
        <input value="" id="firebaseAuth" placeholder="Firebase Auth">
      </div>
      <div>
        <button class="primary" id="savebtn" type="button" onclick="myFunction()">Save</button>
      </div>
     <div>
        <button class="primary" id="settime" type="button" onclick="setTime()">Set Device Time</button>
      </div>
           <div>
        <button class="primary" id="showtime" type="button" onclick="showTime()">Show Device offline Time</button>
      </div>
    </fieldset>
  </form>
</body>
<script>
function myFunction()
{

  var ssid = document.getElementById("ssid").value;
  var password = document.getElementById("password").value;
  var firebaseHost = document.getElementById("firebaseHost").value;
  var firebaseAuth = document.getElementById("firebaseAuth").value;
  var data = {ssid:ssid, password:password, firebaseHost:firebaseHost, firebaseAuth:firebaseAuth};
  var xhr = new XMLHttpRequest();
  var url = "/settings";
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if(xhr.responseText != null){
        console.log(xhr.responseText);
    alert(xhr.responseText);
      }
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(data));
  
};
function showTime()
{

  var xhr = new XMLHttpRequest();
  var url = "/showtime";
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if(xhr.responseText != null){
        console.log(xhr.responseText);
        alert(xhr.responseText);
      }
    }
  };
  xhr.open("POST", url, true);
  xhr.send();
  
};
function addZero(d) {
  if (d < 10){
    d = "0" + d;
  }
  return d;
};
function setTime()
{

  var today = new Date();
  var year = today.getFullYear();
  var mon = new Array();
  mon[0] = 'Jan';
  mon[1] = 'Feb';
  mon[2] = 'Mar';
  mon[3] = 'Apr';
  mon[4] = 'May';
  mon[5] = 'Jun';
  mon[6] = 'Jul';
  mon[7] = 'Aug';
  mon[8] = 'Sep';
  mon[9] = 'Oct';
  mon[10] = 'Nov';
  mon[11] = 'Dec';
  var month = mon[today.getMonth()];
  var date = addZero(today.getDate());
  var hour =  addZero(today.getHours());
  var min =  addZero(today.getMinutes());
  var sec =  addZero(today.getSeconds());  
  var date = month +' '+date+' '+year;
  var time = hour+':'+min+':'+sec; 
  var data = {date:date, time:time};
  var xhr = new XMLHttpRequest();
  var url = "/settime";
  xhr.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if(xhr.responseText != null){
        console.log(xhr.responseText);
        alert(xhr.responseText);
      }
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(data));
  
};
</script>
</html>
)=====";

void doManualTask(FirebaseData &data);
void setup()
{

 Serial.begin(9600);
  SNDPWSTAT = "N";
  pinMode(pinArray[0], OUTPUT);
  pinMode(pinArray[1], OUTPUT);
  pinMode(pinArray[2], OUTPUT);
  pinMode(pinArray[3], OUTPUT);
  pinMode(pinArray[4], OUTPUT);
  pinMode(pinArray[5], OUTPUT);

  SPIFFS.begin();
  Rtc.Begin();
  sensors.begin();
  wifiConnect();  
  
}


void loop()
{  
//resetToAP ();
 if (WiFi.status() == WL_CONNECTED)
  { 
  internet = Ping.ping("www.google.com", 1);
  if(internet) {
    if(Firebase.get(firebaseData, user  "/powers"))
    {
     SNDPWSTAT = "N";
     doManualTask(firebaseData); 
  } } else {SNDPWSTAT = "Y";
            delay(5000);  
           }
 } else {
    SNDPWSTAT = "Y";
    delay(5000);  
 }
 handleOffSch();
 delay(1000);

}
void handleOffSch()
{
    String NTPUNIQUE;
    RtcDateTime now = Rtc.GetDateTime();
   // RtcTemperature temp = Rtc.GetTemperature();
    sensors.requestTemperatures();   
    String Temp = String(sensors.getTempCByIndex(0));
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPMD =  String(now.Day());
    String NPMMON = String(now.Month()- 1);
    String NPMYR = String(now.Year() - 1900);
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = "0" + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = "0" + NPMM; } 
    if (NPMD.length() < 2 ) {NPMD = "0" + NPMD; }
    if (NPMMON.length() < 2 ) {NPMMON = "0" + NPMMON; } 
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;
  if(internet) {
if(Firebase.getString(firebaseData, user "/schedules/SS"))
    {   
if (firebaseData.stringData() == "Y"){
saveSchedule();
}
}
}
     if(Firebase.getString(firebaseData, user "/R"))
    {   
if (firebaseData.stringData() == "Y"){
resetToAP ();
}
} 
  if (NTPUNIQUE != SCHMUNIQUE ){
     SCHMUNIQUE = NTPUNIQUE;
    if(internet) { 
  String FBLOSTA = NPMD + ":";
  FBLOSTA += NPMMON;
  FBLOSTA += ":";
  FBLOSTA += NPMYR;
  FBLOSTA += ":";
  FBLOSTA += NPMH;
  FBLOSTA += ":";
  FBLOSTA += NPMM; 
  Firebase.setString(firebaseData, user "/LO", FBLOSTA); 
 // Firebase.setString(firebaseData, user "/TM", String(temp.AsFloatDegC())); 
  Firebase.setString(firebaseData, user "/TM",Temp );
    }
   
     schTask(NPMH,NPMM,NPWD,Temp);

  }

  if (NPMM.toInt() % 5 == 0 && WiFi.status() != WL_CONNECTED){    
  // wifiConnect();  
    WiFi.reconnect();
  } 
  
}

void doManualTask(FirebaseData &data)
{ 
    if (SNDPWSTAT == String("Y")){
    sendPowerStat();
  SNDPWSTAT = "N";
  return;
  
  }
     
     FirebaseJson &json = data.jsonObject();
     String jsonStr;
     json.toString(jsonStr,true);
     DynamicJsonBuffer jsonBuffer;
     JsonObject& root = jsonBuffer.parseObject(jsonStr);
      String Led1 = root["P_1"]; 
      String Led2 = root["P_2"];  
      String Led3 = root["P_3"]; 
      String Led4 = root["P_4"]; 
      String Led5 = root["P_5"];
      String Led6 = root["P_6"];    
    digitalWrite(pinArray[0], Led1.toInt());
    delay(200);
    digitalWrite(pinArray[1], Led2.toInt());
    delay(200);
    digitalWrite(pinArray[2], Led3.toInt());
    delay(200);
    digitalWrite(pinArray[3], Led4.toInt());
    delay(200);
    digitalWrite(pinArray[4], Led5.toInt());
    delay(200); 
    digitalWrite(pinArray[5], Led6.toInt());
    delay(200);     
}

void startupSync()
{ 

     if(Firebase.getString(firebaseData, user "/powers/P_4"))
  {
    digitalWrite(pinArray[3], firebaseData.stringData().toInt());

  }

  else {   
    Firebase.setString(firebaseData, user "/powers/P_4", "0");
  }
delay(200);
  if(Firebase.getString(firebaseData, user "/powers/P_1"))
  {
    digitalWrite(pinArray[0], firebaseData.stringData().toInt());
  } 
  else {
    Firebase.setString(firebaseData, user "/powers/P_1", "0");
  }
delay(200);
     if(Firebase.getString(firebaseData, user "/powers/P_2"))
  {
    digitalWrite(pinArray[1], firebaseData.stringData().toInt());
  }

  else {
    Firebase.setString(firebaseData, user "/powers/P_2", "0");
  }
delay(200);
  
     if(Firebase.getString(firebaseData, user "/powers/P_3"))
  {
    digitalWrite(pinArray[2], firebaseData.stringData().toInt());
  }

  else {
    Firebase.setString(firebaseData, user "/powers/P_3", "0");
  }
  
delay(200);
if(Firebase.getString(firebaseData, user "/powers/P_5"))
  {
  digitalWrite(pinArray[4], firebaseData.stringData().toInt());
  
  }
  else {
    Firebase.setString(firebaseData, user "/powers/P_5", "0");
  }
delay(200);
if(Firebase.getString(firebaseData, user "/powers/P_6"))
  {
 digitalWrite(pinArray[5], firebaseData.stringData().toInt());
  }
  else {
    Firebase.setString(firebaseData, user "/powers/P_6", "0");
  }
delay(200);

}

void syncScheduleNodes()
{ 
 if(!Firebase.getString(firebaseData, user  "/powers/NC"))
  {
   Firebase.setString(firebaseData, user  "/powers/NC", "OK");
  } 
   if(!Firebase.getString(firebaseData, user  "/LO"))
  {
   Firebase.setString(firebaseData, user "/LO", "");
  } 
  if(!Firebase.getString(firebaseData, user "/TM"))
  {
   Firebase.setString(firebaseData, user "/TM", "");
  } 
  
  if(!Firebase.getString(firebaseData, user "/schedules/S01"))
  {
   Firebase.setString(firebaseData, user "/schedules/S01", "");
  } 
   if(!Firebase.getString(firebaseData, user "/schedules/S02"))
  {
   Firebase.setString(firebaseData, user "/schedules/S02", "");
  } 
    if(!Firebase.getString(firebaseData, user "/schedules/S03"))
  {
   Firebase.setString(firebaseData, user "/schedules/S03", "");
  } 
    if(!Firebase.getString(firebaseData, user "/schedules/S04"))
  {
   Firebase.setString(firebaseData, user "/schedules/S04", "");
  } 
    if(!Firebase.getString(firebaseData, user "/schedules/S05"))
  {
   Firebase.setString(firebaseData, user "/schedules/S05", "");
  } 
    if(!Firebase.getString(firebaseData, user "/schedules/S06"))
  {
   Firebase.setString(firebaseData, user "/schedules/S06", "");
  } 
    if(!Firebase.getString(firebaseData, user "/schedules/S07"))
  {
   Firebase.setString(firebaseData, user "/schedules/S07", "");
  } 
    if(!Firebase.getString(firebaseData, user "/schedules/S08"))
  {
   Firebase.setString(firebaseData, user "/schedules/S08", "");
  } 
   if(!Firebase.getString(firebaseData, user "/schedules/S09"))
  {
   Firebase.setString(firebaseData, user "/schedules/S09", "");
  } 
   if(!Firebase.getString(firebaseData, user "/schedules/S10"))
  {
   Firebase.setString(firebaseData, user "/schedules/S10", "");
  } 
   if(!Firebase.getString(firebaseData, user "/schedules/S11"))
  {
   Firebase.setString(firebaseData, user  "/schedules/S11", "");
  } 
   if(!Firebase.getString(firebaseData, user  "/schedules/S12"))
  {
   Firebase.setString(firebaseData, user  "/schedules/S12", "");
  } 
   if(!Firebase.getString(firebaseData, user  "/schedules/SS"))
  {
   Firebase.setString(firebaseData, user  "/schedules/SS", "N");
  } 
     if(!Firebase.getString(firebaseData, user  "/schedules/ST"))
  {
   Firebase.setString(firebaseData, user  "/schedules/ST", "");
  }
}

void wifiConnect()
{

  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);    
  //WiFi.reconnect();
  delay(2000);
 const char * _ssid = "", *_pass = "", *_firebaseAuth = "", *_firebaseHost = "";
  if(SPIFFS.exists("/config.json")){

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
        _ssid = jObject["ssid"];
        _pass = jObject["password"];
        _firebaseHost = jObject["firebaseHost"];
        _firebaseAuth = jObject["firebaseAuth"];

        WiFi.mode(WIFI_STA);
        WiFi.begin(_ssid, _pass);
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED) 
        {
          delay(500);
         // Serial.print(".");

          if ((unsigned long)(millis() - startTime) >= 10000) break;
        }
  Firebase.begin(_firebaseHost, _firebaseAuth);
  Firebase.reconnectWiFi(true);
  firebaseData.setResponseSize(3072);
  // firebaseData.setBSSLBufferSize(1024, 1024); 
 // WiFi.persistent(true) ;  
  WiFi.setAutoReconnect (true);
 // WiFi.setOutputPower(20.5);
      }
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
   //Serial.println("Connected"); 
  delay (2000);
  internet = Ping.ping("www.google.com", 1);
  if(internet) {
  startupSync();
  syncScheduleNodes();
  } else {
  syncStatus(); 
  }
  } else 
  { 
    syncStatus();    
  }
 // Serial.println("");
 // WiFi.printDiag(Serial);
}

void handleSettingsUpdate()
{
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);

  File configFile = SPIFFS.open("/config.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
  
  server.send(200, "text/html", "Configuration Saved. Please wait,The device will restart automatically");
  delay(2000);
  ESP.restart();
  
}
void setRtcDateTime()
{ 
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
    if(jObject.success())
      { const char *rtcDate = jObject["date"];
        const char *rtcTime = jObject["time"];

        Rtc.SetDateTime(RtcDateTime(rtcDate,rtcTime));
        server.send(200, "text/html", "Device Time has been set successfully");
    }

}
   void sendPowerStat(){
  String p1 = String(digitalRead(pinArray[0]));
  String p2 = String(digitalRead(pinArray[1]));
  String p3 = String(digitalRead(pinArray[2]));
  String p4 = String(digitalRead(pinArray[3]));
  String p5 = String(digitalRead(pinArray[4]));
  String p6 = String(digitalRead(pinArray[5]));
  delay(200);
  Firebase.setString(firebaseData, user "/powers/P_1", p1);
  delay(200);
  Firebase.setString(firebaseData, user "/powers/P_2", p2);
  delay(200);
  Firebase.setString(firebaseData, user "/powers/P_3", p3);
  delay(200);
  Firebase.setString(firebaseData, user "/powers/P_4", p4);
  delay(200);
  Firebase.setString(firebaseData, user "/powers/P_5", p5);
  delay(200);
  Firebase.setString(firebaseData, user "/powers/P_6", p6);
  delay(200);
}
void showRtcTime() {

    RtcDateTime now = Rtc.GetDateTime();
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPMD =  String(now.Day());
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = "0" + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = "0" + NPMM; } 
  
  String conDate = NPMH + ":";
  conDate = conDate + NPMM;
  
  server.send(200, "text/html", "Offline Device Time: " + conDate);
       
}  
void handleTemp(String temp, String tempPm)
{  
     String tempChar = temp.substring(0,5) ;
   String type = temp.substring(6);
   float tempF = tempChar.toFloat() ;
   float tempfm = tempPm.toFloat() ;
   if (type == "Up") {
    if(tempfm > tempF){
      //turn on
      digitalWrite(pinArray[3], 1);
      Firebase.setString(firebaseData, user  "/powers/P_4", "1");
    }else {
      //turn off
      digitalWrite(pinArray[3], 0);
      Firebase.setString(firebaseData, user  "/powers/P_4", "0");
    } 
     
   } else {
     if(tempfm < tempF){
       //turn on
      digitalWrite(pinArray[3], 1);
      Firebase.setString(firebaseData, user  "/powers/P_4", "1");
     }else {
      // turn off
      digitalWrite(pinArray[3], 0);
      Firebase.setString(firebaseData, user  "/powers/P_4", "0");
     } 
   }
}
void handleMilis(String power, String duration)
{  
       int durationInt = duration.toInt();
     durationInt = durationInt * 1000;     
       unsigned long startTime = millis();
        while (1!= 2) 
        {
      digitalWrite(pinArray[ power.toInt() - 1 ], 1);
          delay(1000);
          if ((unsigned long)(millis() - startTime) >= durationInt) {
        digitalWrite(pinArray[ power.toInt() - 1 ], 0);
        break;}
        }

}
void resetToAP ()
{   
 Serial.println("Turning On Access Point");
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
    server.on("/settings", HTTP_POST, handleSettingsUpdate);
  server.on("/settime", HTTP_POST, setRtcDateTime);
  server.on("/showtime", HTTP_POST, showRtcTime);
    server.begin(); 
    delay(1000);  
    while (1 != 2) 
        {
           server.handleClient();
   
             delay(200);
        }
    
}
void checkSchedule(String sch, String hh, String mn, String dy)
{  
if (hh == sch.substring(4,6) && mn == sch.substring(7,9) && (sch.substring(13).indexOf(dy) >= 0 || sch.substring(13).indexOf("7") >= 0)    ){
    
      if (sch.substring(10,12) != "--"){
     handleMilis( sch.substring(0,1), sch.substring(10,12));
    // Serial.print("Handle Time Null");
    } else{   
      digitalWrite(pinArray[ sch.substring(0,1).toInt() - 1 ], sch.substring(2,3).toInt());
    delay(200);
  if(internet) {
    String  firebasefPath = firebasePath +  sch.substring(0,1);
      Firebase.setString(firebaseData, firebasefPath, sch.substring(2,3));
  }
    }
    
       }

}
void schTask(String NPMH, String NPMM, String NPWD, String Temp){ 
 if(SPIFFS.exists("/schdl.json")){
    
    File configFile = SPIFFS.open("/schdl.json", "r");
    if(configFile){
      size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();

      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(buf.get());
      if(root.success())
      {
      String Sch1  = root["S01"]; 
      String Sch2  = root["S02"];  
      String Sch3  = root["S03"]; 
      String Sch4  = root["S04"]; 
      String Sch5  = root["S05"]; 
      String Sch6  = root["S06"]; 
      String Sch7  = root["S07"]; 
      String Sch8  = root["S08"]; 
      String Sch9  = root["S09"];
      String Sch10 = root["S10"];
      String Sch11 = root["S11"];
      String Sch12 = root["S12"]; 
      String Sstemp = root["ST"];    
    checkSchedule(Sch1, NPMH, NPMM,NPWD );
    checkSchedule(Sch2, NPMH, NPMM,NPWD );
    checkSchedule(Sch3, NPMH, NPMM,NPWD );
    checkSchedule(Sch4, NPMH, NPMM,NPWD );
    checkSchedule(Sch5, NPMH, NPMM,NPWD );
    checkSchedule(Sch6, NPMH, NPMM,NPWD );
    checkSchedule(Sch7, NPMH, NPMM,NPWD );
    checkSchedule(Sch8, NPMH, NPMM,NPWD );
    checkSchedule(Sch9, NPMH, NPMM,NPWD );
    checkSchedule(Sch10, NPMH, NPMM,NPWD );
    checkSchedule(Sch11, NPMH, NPMM,NPWD );
    checkSchedule(Sch12, NPMH, NPMM,NPWD ); 
    if (!Sstemp.isEmpty()) 
{
 handleTemp(Sstemp, Temp);
}
}
  }
   }
     if (NPMM.toInt() % 5 == 0){
     saveStatus();    
  }
}
void saveSchedule(){
      if(Firebase.get(firebaseData, user  "/schedules"))
    {
  FirebaseJson &json = firebaseData.jsonObject(); 
  String jsonStr;
  json.toString(jsonStr,true);
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(jsonStr);
  File configFile = SPIFFS.open("/schdl.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
  Firebase.setString(firebaseData, user  "/schedules/SS", "N");
 //Serial.println("Saved"); 
  }
}
void saveStatus()
{  
 DynamicJsonBuffer jsonBuffer;
 JsonObject& power = jsonBuffer.createObject();
 delay(200);
 power["P1"] = String(digitalRead(pinArray[0]));
 delay(200);
 power["P2"] = String(digitalRead(pinArray[1]));
 delay(200);
 power["P3"] = String(digitalRead(pinArray[2]));
 delay(200);
 power["P4"] = String(digitalRead(pinArray[3]));
 delay(200);
 power["P5"] = String(digitalRead(pinArray[4]));
 delay(200);
 power["P6"] = String(digitalRead(pinArray[5]));
 delay(200);

 File configFile = SPIFFS.open("/saveStat.json", "w");
 power.printTo(configFile);  
 configFile.close();
 delay(200);
}
void syncStatus()
{  
if(SPIFFS.exists("/saveStat.json")){
    
    File configFile = SPIFFS.open("/saveStat.json", "r");
    if(configFile){
    size_t size = configFile.size();
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      configFile.close();

      DynamicJsonBuffer jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(buf.get());
          if(root.success())
      {
      String P1  = root["P1"]; 
      String P2  = root["P2"];  
      String P3  = root["P3"]; 
      String P4  = root["P4"];    
      String P5  = root["P5"]; 
      String P6  = root["P6"];    
  delay(200);
    digitalWrite(pinArray[0], P1.toInt());
    delay(200);
    digitalWrite(pinArray[1], P2.toInt());
    delay(200);
    digitalWrite(pinArray[2], P3.toInt());
    delay(200);
    digitalWrite(pinArray[3], P4.toInt());
    delay(200);
    digitalWrite(pinArray[4], P5.toInt());
    delay(200); 
    digitalWrite(pinArray[5], P6.toInt());
    delay(200);   
    }
    
  }
}
}
