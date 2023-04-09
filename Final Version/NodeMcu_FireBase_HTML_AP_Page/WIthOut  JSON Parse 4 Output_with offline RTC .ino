#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <RtcDS3231.h>


String user = String( "/AqLnUw9xO"); 
String firebasePath= String ("/P_");
ESP8266WebServer server;
char* mySsid   = "AquaLink";
char* password = "123456789";
int timezone = 19800;
WiFiUDP ntpUDP;
RtcDS3231<TwoWire> Rtc(Wire);
NTPClient timeClient(ntpUDP, "pool.ntp.org");
FirebaseData firebaseData;
// SDA --> D2 (GPIO 4)
// SCL --> D1 (GPIO 5)  RTC 
const int pin_led = 2;//D4
const int set_pin = 16; // D0
const String add0 = "0";
int pinArray[] = {14,12,13,15}; //D5,D6,D7,D8

String FBuQ;
String NTPUNIQUE;
String SCHMUNIQUE;
String SNDPWSTAT;

String firebasefPath;

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


void setup()
{
 
 Serial.begin(115200);
 SNDPWSTAT = 'N';
  pinMode(pinArray[0], OUTPUT);
  pinMode(pinArray[1], OUTPUT);
  pinMode(pinArray[2], OUTPUT);
  pinMode(pinArray[3], OUTPUT);
  pinMode(set_pin, INPUT);
  pinMode(pin_led, OUTPUT);
  SPIFFS.begin();
  Rtc.Begin();
  digitalWrite(set_pin,LOW);

wifiConnect();
   
  timeClient.begin();
  timeClient.setTimeOffset(timezone);
 Firebase.reconnectWiFi(true);
 firebasePath = user + firebasePath;
 startupSync();
 syncScheduleNodes();
  
}


void loop()
{  

if ( digitalRead(set_pin) == HIGH ) {
   resetToAP ();
   
 } 
 else {  
 if (WiFi.status() == WL_CONNECTED)
  { // digitalWrite(pin_led,HIGH);
 
    if(Firebase.getString(firebaseData, user + "/NC"))
    {
  doManualTask(); 
  checkSchedule();
 } else {handleOffSch();
           }
 } else {
    handleOffSch();
    digitalWrite(pin_led,!digitalRead(pin_led));
  delay(600);
   
 }
 }
 
}

void checkSchedule ()
{ String Sch1;
String Sch2;
String Sch3;
String Sch4;
String Sch5;
String Sch6;
String Sch7;
String Sch8;
String Sch9;
String Sch10;
String Sch11;
String Sch12;
String SSch;
    timeClient.update();
    unsigned long epochTime = timeClient.getEpochTime();
    struct tm *p_tm = gmtime ((time_t *)&epochTime); 
    String NPMMON = String(p_tm->tm_mon) ;
    String NPMYR  = String(p_tm->tm_year) ;
    String NPMH = String(p_tm->tm_hour) ;
    String NPMM = String(p_tm->tm_min);
    String NPMD = String(p_tm->tm_mday);
  String NPWD = String(p_tm->tm_wday) ; 
  if (NPMH.length() < 2 ) {NPMH = add0 + NPMH; }
  if (NPMM.length() < 2 ) {NPMM = add0 + NPMM; }
  if (NPMD.length() < 2 ) {NPMD = add0 + NPMD; }
  if (NPMMON.length() < 2 ) {NPMMON = add0 + NPMMON; }
  
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;
    
  
    if (NTPUNIQUE != FBuQ ){
      
 String FBLOSTA = NPMD + ":";
  FBLOSTA += NPMMON;
  FBLOSTA += ":";
  FBLOSTA += NPMYR;
  FBLOSTA += ":";
  FBLOSTA += NPMH;
  FBLOSTA += ":";
  FBLOSTA += NPMM;     
     //------------------------------
      
     if(Firebase.getString(firebaseData, user + "/S01"))
  {
    Sch1 = firebaseData.stringData();
  } 
   if(Firebase.getString(firebaseData, user + "/S02"))
  {
   Sch2 = firebaseData.stringData();
  } 
    if(Firebase.getString(firebaseData, user + "/S03"))
  {
   Sch3 = firebaseData.stringData();
  } 
    if(Firebase.getString(firebaseData, user + "/S04"))
  {
    Sch4 = firebaseData.stringData();
  } 
    if(Firebase.getString(firebaseData, user + "/S05"))
  {
    Sch5 = firebaseData.stringData();
  } 
    if(Firebase.getString(firebaseData, user + "/S06"))
  {
    Sch6 = firebaseData.stringData();
  } 
    if(Firebase.getString(firebaseData, user + "/S07"))
  {
    Sch7 = firebaseData.stringData();
  } 
    if(Firebase.getString(firebaseData, user + "/S08"))
  {
    Sch8 = firebaseData.stringData();
  } 
   if(Firebase.getString(firebaseData, user + "/S09"))
  {
    Sch9 = firebaseData.stringData();
  } 
   if(Firebase.getString(firebaseData, user + "/S10"))
  {
   Sch10 = firebaseData.stringData();
  } 
   if(Firebase.getString(firebaseData, user + "/S11"))
  {
    Sch11 = firebaseData.stringData();
  } 
   if(Firebase.getString(firebaseData, user + "/S12"))
  {
    Sch12 = firebaseData.stringData();
  } 
     if(Firebase.getString(firebaseData, user + "/SS"))
  {
    SSch = firebaseData.stringData();
  } 
    
    
     //------------------------------

    if (NPMH == Sch1.substring(4,6) && NPMM == Sch1.substring(7,9) && Sch1.substring(9).indexOf(NPWD) >= 0   ){

      firebasefPath = firebasePath + Sch1.substring(0,1);
      digitalWrite(pinArray[ Sch1.substring(0,1).toInt() - 1 ], Sch1.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch1.substring(2,3));
       }

  //-----------------------------------

    if (NPMH == Sch2.substring(4,6) && NPMM == Sch2.substring(7,9)&& Sch2.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch2.substring(0,1);
      digitalWrite(pinArray[ Sch2.substring(0,1).toInt() - 1 ], Sch2.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch2.substring(2,3));
      
  }
  
    //-----------------------------------
    if (NPMH == Sch3.substring(4,6) && NPMM == Sch3.substring(7,9)&& Sch3.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch3.substring(0,1);
      digitalWrite(pinArray[ Sch3.substring(0,1).toInt() - 1 ], Sch3.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch3.substring(2,3));
      
  }

    //-----------------------------------
    if (NPMH == Sch4.substring(4,6) && NPMM == Sch4.substring(7,9) && Sch4.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch4.substring(0,1);
      digitalWrite(pinArray[ Sch4.substring(0,1).toInt() - 1 ], Sch4.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch4.substring(2,3));
      
  }
  
    //-----------------------------------

    if (NPMH == Sch5.substring(4,6) && NPMM == Sch5.substring(7,9) && Sch5.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch5.substring(0,1);
      digitalWrite(pinArray[ Sch5.substring(0,1).toInt() - 1 ], Sch5.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch5.substring(2,3));
      
  }
  
    //-----------------------------------
    if (NPMH == Sch6.substring(4,6) && NPMM == Sch6.substring(7,9) && Sch6.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch6.substring(0,1);
      digitalWrite(pinArray[ Sch6.substring(0,1).toInt() - 1 ], Sch6.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch6.substring(2,3));
     
  }
  
    //-----------------------------------

    if (NPMH == Sch7.substring(4,6) && NPMM == Sch7.substring(7,9) && Sch7.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch7.substring(0,1);
      digitalWrite(pinArray[ Sch7.substring(0,1).toInt() - 1 ], Sch7.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch7.substring(2,3));
      
  }
  
    //-----------------------------------

    if (NPMH == Sch8.substring(4,6) && NPMM == Sch8.substring(7,9) && Sch8.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch8.substring(0,1);
      digitalWrite(pinArray[ Sch8.substring(0,1).toInt() - 1 ], Sch8.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch8.substring(2,3));
      
  }
    //-----------------------------------

    if (NPMH == Sch9.substring(4,6) && NPMM == Sch9.substring(7,9) && Sch9.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch9.substring(0,1);
      digitalWrite(pinArray[ Sch9.substring(0,1).toInt() - 1 ], Sch9.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch9.substring(2,3));
      
  }
 //-----------------------------------

    if (NPMH == Sch10.substring(4,6) && NPMM == Sch10.substring(7,9) && Sch10.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch10.substring(0,1);
      digitalWrite(pinArray[ Sch10.substring(0,1).toInt() - 1 ], Sch10.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch10.substring(2,3));
      
  }
  //-----------------------------------

    if (NPMH == Sch11.substring(4,6) && NPMM == Sch11.substring(7,9) && Sch11.substring(9).indexOf(NPWD) >= 0  ){
      
      firebasefPath = firebasePath + Sch11.substring(0,1);
      digitalWrite(pinArray[ Sch11.substring(0,1).toInt() - 1 ], Sch11.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch11.substring(2,3));
      
  }
  //-----------------------------------

    if (NPMH == Sch12.substring(4,6) && NPMM == Sch12.substring(7,9) && Sch12.substring(9).indexOf(NPWD) >= 0   ){
      
      firebasefPath = firebasePath + Sch12.substring(0,1);
      digitalWrite(pinArray[ Sch12.substring(0,1).toInt() - 1 ], Sch12.substring(2,3).toInt());
      Firebase.setString(firebaseData, firebasefPath, Sch12.substring(2,3));
      
  }
  Firebase.setString(firebaseData, user + "/LO", FBLOSTA);
    FBuQ =NTPUNIQUE;
  
  if (SSch =="Y") 
{
 DynamicJsonBuffer jsonBuffer;
 JsonObject& schedule = jsonBuffer.createObject();
 schedule["S01"] = Sch1;
 schedule["S02"] = Sch2;
 schedule["S03"] = Sch3;
 schedule["S04"] = Sch4;
 schedule["S05"] = Sch5;
 schedule["S06"] = Sch6;
 schedule["S07"] = Sch7;
 schedule["S08"] = Sch8;
 schedule["S09"] = Sch9;
 schedule["S10"] = Sch10;
 schedule["S11"] = Sch11;
 schedule["S12"] = Sch12;
 File configFile = SPIFFS.open("/schdl.json", "w");
 schedule.printTo(configFile);  
 configFile.close();
 Firebase.setString(firebaseData, user + "/SS", "N");
 Serial.print("Schedules Saved in SPIFFS");
}
  
  
  
  
    }
}
void handleOffSch()
{
  SNDPWSTAT = 'Y';
    RtcDateTime now = Rtc.GetDateTime();
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPMD =  String(now.Day());
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = '0' + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = '0' + NPMM; }   
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;

  Serial.println(NTPUNIQUE);
  if (NTPUNIQUE != SCHMUNIQUE ){
      SCHMUNIQUE = NTPUNIQUE;
     
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
  
    //------------------------------

    if (NPMH == Sch1.substring(4,6) && NPMM == Sch1.substring(7,9) && Sch1.substring(9).indexOf(NPWD) >= 0 ){

      digitalWrite(pinArray[ Sch1.substring(0,1).toInt() - 1 ], Sch1.substring(2,3).toInt());

       }

  //-----------------------------------

    if (NPMH == Sch2.substring(4,6) && NPMM == Sch2.substring(7,9)&& Sch2.substring(9).indexOf(NPWD) >= 0   ){
      
      digitalWrite(pinArray[ Sch2.substring(0,1).toInt() - 1 ], Sch2.substring(2,3).toInt());
  }
  
    //-----------------------------------
    if (NPMH == Sch3.substring(4,6) && NPMM == Sch3.substring(7,9)&& Sch3.substring(9).indexOf(NPWD) >= 0  ){
      
      digitalWrite(pinArray[ Sch3.substring(0,1).toInt() - 1 ], Sch3.substring(2,3).toInt());
  }

    //-----------------------------------
    if (NPMH == Sch4.substring(4,6) && NPMM == Sch4.substring(7,9) && Sch4.substring(9).indexOf(NPWD) >= 0  ){
      
      digitalWrite(pinArray[ Sch4.substring(0,1).toInt() - 1 ], Sch4.substring(2,3).toInt());

  }
  
    //-----------------------------------

    if (NPMH == Sch5.substring(4,6) && NPMM == Sch5.substring(7,9) && Sch5.substring(9).indexOf(NPWD) >= 0  ){
      
      digitalWrite(pinArray[ Sch5.substring(0,1).toInt() - 1 ], Sch5.substring(2,3).toInt());

  }
  
    //-----------------------------------
    if (NPMH == Sch6.substring(4,6) && NPMM == Sch6.substring(7,9) && Sch6.substring(9).indexOf(NPWD) >= 0  ){

      digitalWrite(pinArray[ Sch6.substring(0,1).toInt() - 1 ], Sch6.substring(2,3).toInt());

  }
  
    //-----------------------------------

    if (NPMH == Sch7.substring(4,6) && NPMM == Sch7.substring(7,9) && Sch7.substring(9).indexOf(NPWD) >= 0  ){
      

      digitalWrite(pinArray[ Sch7.substring(0,1).toInt() - 1 ], Sch7.substring(2,3).toInt());

  }
  
    //-----------------------------------

    if (NPMH == Sch8.substring(4,6) && NPMM == Sch8.substring(7,9) && Sch8.substring(9).indexOf(NPWD) >= 0  ){

      digitalWrite(pinArray[ Sch8.substring(0,1).toInt() - 1 ], Sch8.substring(2,3).toInt());

  }
    //-----------------------------------

    if (NPMH == Sch9.substring(4,6) && NPMM == Sch9.substring(7,9) && Sch9.substring(9).indexOf(NPWD) >= 0   ){

      digitalWrite(pinArray[ Sch9.substring(0,1).toInt() - 1 ], Sch9.substring(2,3).toInt());

  }
 //-----------------------------------

    if (NPMH == Sch10.substring(4,6) && NPMM == Sch10.substring(7,9) && Sch10.substring(9).indexOf(NPWD) >= 0 ){
      
      digitalWrite(pinArray[ Sch10.substring(0,1).toInt() - 1 ], Sch10.substring(2,3).toInt());

  }
  //-----------------------------------

    if (NPMH == Sch11.substring(4,6) && NPMM == Sch11.substring(7,9) && Sch11.substring(9).indexOf(NPWD) >= 0 ){
      
      digitalWrite(pinArray[ Sch11.substring(0,1).toInt() - 1 ], Sch11.substring(2,3).toInt());
  }
  //-----------------------------------

    if (NPMH == Sch12.substring(4,6) && NPMM == Sch12.substring(7,9) && Sch12.substring(9).indexOf(NPWD) >= 0){

      digitalWrite(pinArray[ Sch12.substring(0,1).toInt() - 1 ], Sch12.substring(2,3).toInt());

     }
}
  }
   }
  }
}
void doManualTask()
{ 
    if (SNDPWSTAT == String('Y')){
    sendPowerStat();
  }
      SNDPWSTAT = 'N';
if(Firebase.getString(firebaseData, user + "/P_1"))
  {
    digitalWrite(pinArray[0], firebaseData.stringData().toInt());
  }   
if(Firebase.getString(firebaseData, user + "/P_2"))
  {
    digitalWrite(pinArray[1], firebaseData.stringData().toInt());
  } 
  if(Firebase.getString(firebaseData, user + "/P_3"))
  {
    digitalWrite(pinArray[2], firebaseData.stringData().toInt());
  } 
  if(Firebase.getString(firebaseData, user + "/P_4"))
  {
    digitalWrite(pinArray[3], firebaseData.stringData().toInt());
  } 

     
}

void startupSync()
{ 


  if(Firebase.getString(firebaseData, user + "/P_1"))
  {
    digitalWrite(pinArray[0], firebaseData.stringData().toInt());
  } 
  else {
    Firebase.setString(firebaseData, user + "/P_1", "0");
  }
  
     if(Firebase.getString(firebaseData, user + "/P_2"))
  {
    digitalWrite(pinArray[1], firebaseData.stringData().toInt());
  }

  else {
    Firebase.setString(firebaseData, user + "/P_2", "0");
  }
  

     if(Firebase.getString(firebaseData, user + "/P_3"))
  {
    digitalWrite(pinArray[2], firebaseData.stringData().toInt());
  }

  else {
    Firebase.setString(firebaseData, user + "/P_3", "0");
  }
  

     if(Firebase.getString(firebaseData, user + "/P_4"))
  {
    digitalWrite(pinArray[3], firebaseData.stringData().toInt());

  }
  

  else {
    Firebase.setString(firebaseData, user + "/P_4", "0");
  }

if(Firebase.getString(firebaseData, user + "/P_5"))
  {
   // digitalWrite(pinArray[3], firebaseData.stringData().toInt());
   Serial.println("P_5"); 

  }
  

  else {
    Firebase.setString(firebaseData, user + "/P_5", "0");
  }

if(Firebase.getString(firebaseData, user + "/P_6"))
  {
   // digitalWrite(pinArray[3], firebaseData.stringData().toInt());
   Serial.println("P_6"); 
  }
  

  else {
    Firebase.setString(firebaseData, user + "/P_6", "0");
  }


}

void syncScheduleNodes()
{ 
 if(!Firebase.getString(firebaseData, user + "/NC"))
  {
   Firebase.setString(firebaseData, user + "/NC", "OK");
  } 
   if(!Firebase.getString(firebaseData, user + "/LO"))
  {
   Firebase.setString(firebaseData, user + "/LO", "");
  } 
  
  if(!Firebase.getString(firebaseData, user + "/S01"))
  {
   Firebase.setString(firebaseData, user + "/S01", "");
  } 
   if(!Firebase.getString(firebaseData, user + "/S02"))
  {
   Firebase.setString(firebaseData, user + "/S02", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/S03"))
  {
   Firebase.setString(firebaseData, user + "/S03", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/S04"))
  {
   Firebase.setString(firebaseData, user + "/S04", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/S05"))
  {
   Firebase.setString(firebaseData, user + "/S05", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/S06"))
  {
   Firebase.setString(firebaseData, user + "/S06", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/S07"))
  {
   Firebase.setString(firebaseData, user + "/S07", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/S08"))
  {
   Firebase.setString(firebaseData, user + "/S08", "");
  } 
   if(!Firebase.getString(firebaseData, user + "/S09"))
  {
   Firebase.setString(firebaseData, user + "/S09", "");
  } 
   if(!Firebase.getString(firebaseData, user + "/S10"))
  {
   Firebase.setString(firebaseData, user + "/S10", "");
  } 
   if(!Firebase.getString(firebaseData, user + "/S11"))
  {
   Firebase.setString(firebaseData, user + "/S11", "");
  } 
   if(!Firebase.getString(firebaseData, user + "/S12"))
  {
   Firebase.setString(firebaseData, user + "/S12", "");
  } 
  if(!Firebase.getString(firebaseData, user + "/SS"))
  {
   Firebase.setString(firebaseData, user + "/SS", "N");
  } 
}

void wifiConnect()
{

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();          
  delay(1000);

  if(SPIFFS.exists("/config.json")){
    const char * _ssid = "", *_pass = "", *_firebaseAuth = "", *_firebaseHost = "";
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
          Serial.print(".");
          digitalWrite(pin_led,!digitalRead(pin_led));
          if ((unsigned long)(millis() - startTime) >= 20000) break;
        }
    Firebase.begin(_firebaseHost, _firebaseAuth);
      }
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
   Serial.println("Connected"); 
    digitalWrite(pin_led,HIGH);
  } else 
  { digitalWrite(pin_led,LOW);    
  }
  Serial.println("");
  WiFi.printDiag(Serial);
}

void handleSettingsUpdate()
{
  String data = server.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);

  File configFile = SPIFFS.open("/config.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
  
  server.send(200, "text/html", "Configuration Saved. Please Restart the Device");
  
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
  
  Firebase.setString(firebaseData, user + "/P_1", p1);
  Firebase.setString(firebaseData, user + "/P_2", p2);
  Firebase.setString(firebaseData, user + "/P_3", p3);
  Firebase.setString(firebaseData, user + "/P_4", p4);
}
void showRtcTime() {

    RtcDateTime now = Rtc.GetDateTime();
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPMD =  String(now.Day());
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = '0' + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = '0' + NPMM; } 
  
  String conDate = NPMH + ":";
  conDate = conDate + NPMM;
  
  server.send(200, "text/html", "Offline Device Time: " + conDate);
       
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
               digitalWrite(pin_led,!digitalRead(pin_led));
             delay(200);
        }
    
}
