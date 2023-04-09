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
String schList [12];
int schintarr [12][4];
boolean internet;


void doManualTask(FirebaseData &data);
void setup()
{

  //Serial.begin(9600);
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
            delay(1000);  
           }
 } else {
    SNDPWSTAT = "Y";
    delay(1000);  
 }
 handleOffSch();
 delay(500);

}
void handleOffSch()
{
    String NTPUNIQUE;
    RtcDateTime now = Rtc.GetDateTime();
   // RtcTemperature temp = Rtc.GetTemperature();
    sensors.requestTemperatures();   
    String Temp = String(sensors.getTempCByIndex(0));
    String NPMH ;
    String NPMM;
    String NPMD ;
    String NPMMON ;
    String NPMYR = String(now.Year() - 1900);
    String NPWD =  String(now.DayOfWeek());

    NPMH = addZero(now.Hour());
  NPMM = addZero(now.Minute());
  NPMD = addZero(now.Day());
  NPMMON = addZero(now.Month()- 1);
    //   Serial.println(NPMH);
    // Serial.println(NPMM);
    // Serial.println(NPMD);
    // Serial.println(NPMMON);
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;

  if (NTPUNIQUE != SCHMUNIQUE ){
     SCHMUNIQUE = NTPUNIQUE;
    if(internet) {


if(Firebase.getString(firebaseData, user "/schedules/SS"))
    {   
if (firebaseData.stringData() == "Y"){
saveSchedule();
}
}
     if(Firebase.getString(firebaseData, user "/R"))
    {   
if (firebaseData.stringData() == "Y"){
resetToAP ();
}
} 
		
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
    delay(20);
    digitalWrite(pinArray[1], Led2.toInt());
    delay(20);
    digitalWrite(pinArray[2], Led3.toInt());
    delay(20);
    digitalWrite(pinArray[3], Led4.toInt());
    delay(20);
    digitalWrite(pinArray[4], Led5.toInt());
    delay(20); 
    digitalWrite(pinArray[5], Led6.toInt());
    delay(20);     
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
       if(!Firebase.getString(firebaseData, user  "/schedules/MD"))
  {
   Firebase.setString(firebaseData, user  "/schedules/MD", "1");
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
  delay(20);
  Firebase.setString(firebaseData, user "/powers/P_1", p1);
  delay(20);
  Firebase.setString(firebaseData, user "/powers/P_2", p2);
  delay(20);
  Firebase.setString(firebaseData, user "/powers/P_3", p3);
  delay(20);
  Firebase.setString(firebaseData, user "/powers/P_4", p4);
  delay(20);
  Firebase.setString(firebaseData, user "/powers/P_5", p5);
  delay(20);
  Firebase.setString(firebaseData, user "/powers/P_6", p6);
  delay(20);
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
   // server.on("/",[](){server.send_P(200,"text/html", webpage);});
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
void checkScheduleNew(String scharr[], String hh, String mn, String dy) {

  String schtimechar;
  String currtimechar;
  int  currtimeint;
  int  prevscheduleint;
  int  power [6]={0,0,0,0,0,0};
  
  currtimechar = hh;
  currtimechar += mn;
  currtimeint = currtimechar.toInt();
  for (int i = 0; i < 12; i++) {
    
    schtimechar = scharr[i].substring(4, 6) + scharr[i].substring(7, 9); //hhmi
    schintarr[i][0] = scharr[i].substring(0, 1).toInt();   //powerpoint
    schintarr[i][1] = scharr[i].substring(2, 3).toInt();   // state
    schintarr[i][2] = schtimechar.toInt(); //hhmi
  
    if (scharr[i].substring(13).indexOf(dy) >= 0 || scharr[i].substring(13).indexOf("7") >= 0) {
      schintarr[i][3] = 1;     //Active Day
    } else {
      schintarr[i][3] = 0;     //Inactive Day
    }
  }
  
  for (int p = 0; p < 6; p++) {//for each powerpoint
      prevscheduleint = 0;
    for (int j = 0; j < 12; j++) {//for each schedule     
      if (schintarr[j][3] == 1 && schintarr[j][0] == (p + 1))  {
        if (currtimeint >= schintarr[j][2] &&  schintarr[j][2] >= prevscheduleint) {
          power[p] = schintarr[j][1];
          prevscheduleint = schintarr[j][2];
        }

      }
    
    }
  
    digitalWrite(pinArray[p], power[p]);
    delay(20);
       internet = Ping.ping("www.google.com", 1);
     delay(20);
      if (internet) {
        int powerNode = p + 1;
        String firebasefPath = firebasePath + String(powerNode);
        Firebase.setString(firebaseData, firebasefPath, String(power[p]));
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
 
     String Mode = root["MD"]; 
     String Sstemp = root["ST"];          

    if(Mode.toInt() == 2) {
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

 schList[0]  = Sch1;
  schList[1]  = Sch2;
   schList[2]  = Sch3;
    schList[3]  = Sch4;
     schList[4]  = Sch5;
      schList[5]  = Sch6;
       schList[6]  = Sch7;
        schList[7]  = Sch8;
         schList[8]  = Sch9;
          schList[9]  = Sch10;
           schList[10]  = Sch11;
            schList[11]  = Sch12;
checkScheduleNew(schList, NPMH, NPMM,NPWD);
    }
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

String  addZero(int d) {
  String val;
  if (d < 10){
    val = "0" + String(d);
  } else {val = String(d);}
  return val;
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
 delay(20);
 power["P1"] = String(digitalRead(pinArray[0]));
 delay(20);
 power["P2"] = String(digitalRead(pinArray[1]));
 delay(20);
 power["P3"] = String(digitalRead(pinArray[2]));
 delay(20);
 power["P4"] = String(digitalRead(pinArray[3]));
 delay(20);
 power["P5"] = String(digitalRead(pinArray[4]));
 delay(20);
 power["P6"] = String(digitalRead(pinArray[5]));
 delay(20);

 File configFile = SPIFFS.open("/saveStat.json", "w");
 power.printTo(configFile);  
 configFile.close();
 delay(20);
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
    delay(20);
    digitalWrite(pinArray[0], P1.toInt());
    delay(20);
    digitalWrite(pinArray[1], P2.toInt());
    delay(20);
    digitalWrite(pinArray[2], P3.toInt());
    delay(20);
    digitalWrite(pinArray[3], P4.toInt());
    delay(20);
    digitalWrite(pinArray[4], P5.toInt());
    delay(20); 
    digitalWrite(pinArray[5], P6.toInt());
    delay(20);   
    }
    
  }
}
}
