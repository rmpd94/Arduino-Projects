#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#define ONE_WIRE_BUS 2                          //D4 pin of nodemcu

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid     = "AquaLink_D002";
const char* password = "Paradise";
String deviceId     = String( "/Paradise");

String setngPath     = String( "/schedules");
String adhocOpPath   = String( "/operations");
String timeNow       = String( "/timenow");
String powerStat     = String( "/powerstat");
String setDateTime   = String( "/setdatetime");
int pinArray[] = {16,0,14,12,13,15}; //D0,D3,D5,D6,D7,D8

ESP8266WebServer server1;
RtcDS3231<TwoWire> Rtc(Wire);
String SCHMUNIQUE;

void setup() {
  Serial.begin(9600);
  pinMode(pinArray[0], OUTPUT);
  pinMode(pinArray[1], OUTPUT);
  pinMode(pinArray[2], OUTPUT);
  pinMode(pinArray[3], OUTPUT);
  pinMode(pinArray[4], OUTPUT);
  pinMode(pinArray[5], OUTPUT);

  SPIFFS.begin();
  Rtc.Begin();
  sensors.begin();
  
  Serial.print("Setting AP (Access Point)…");
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server1.on(deviceId + setngPath,  HTTP_POST, handleScheduleUpdate);
  server1.on(deviceId + adhocOpPath, HTTP_POST, handleAdhocTask);
  server1.on(deviceId + timeNow,    HTTP_POST, sendTime);
  server1.on(deviceId + powerStat,  HTTP_POST, sendPowerStat);
  server1.on(deviceId + setDateTime,  HTTP_POST, setRtcDateTime);
  server1.begin();
  syncStatus();
}

void loop(){
  server1.handleClient(); 
  handleSchedule(); 
  delay(500);
}

void handleScheduleUpdate()
{
  String data = server1.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
  
  File configFile = SPIFFS.open("/schedule.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
  server1.send(200, "application/json", "{\"status\" : \"ok\"}");
  }


void handleAdhocTask()
{ 
  String data = server1.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
    if(jObject.success())
      { String power     = jObject["power"];
        String operation = jObject["operation"];
     digitalWrite(pinArray[ power.toInt()], operation.toInt());
	 
    }
  
  server1.send(200, "application/json", "{\"status\" : \"ok\"}"); 
}

void setRtcDateTime()
{ 
  String data = server1.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);
    if(jObject.success())
      { const char *rtcDate = jObject["date"];
        const char *rtcTime = jObject["time"];
        Rtc.SetDateTime(RtcDateTime(rtcDate,rtcTime));
        server1.send(200, "application/json", "{\"status\" : \"ok\"}");
    }
    
}
void sendTime(){
        String Temp;
    String TIMENOW;
    String DTTIME;
  Temp = String(sensors.getTempCByIndex(0));
  RtcDateTime rtctm = Rtc.GetDateTime();
  String  ADNPMH =  String(rtctm.Hour());
  String  ADNPMM =  String(rtctm.Minute());
  String  ADNPMD =  String(rtctm.Day());
  String  ADNPMN =  String(rtctm.Month());
  String  ADNPMY =  String(rtctm.Year());

  if (ADNPMH.length() < 2 ) {ADNPMH = "0" + ADNPMH; }
  if (ADNPMM.length() < 2 ) {ADNPMM = "0" + ADNPMM; }
  if (ADNPMN.length() < 2 ) {ADNPMN = "0" + ADNPMN; }
  if (ADNPMD.length() < 2 ) {ADNPMD = "0" + ADNPMD; }

  DTTIME  = ADNPMD + ":";
  DTTIME += ADNPMN;
  DTTIME += ":";
  DTTIME += ADNPMY;
  DTTIME += ":";
  DTTIME += ADNPMH;
  DTTIME += ":";
  DTTIME += ADNPMM;
  TIMENOW  = "{\"timeNow\" : \"";
  TIMENOW += DTTIME;
  TIMENOW += "\", ";
  TIMENOW += "\"temp\" : \"";
  TIMENOW += Temp;
  TIMENOW += "\"}";
  
  //Serial.print(TIMENOW);
  server1.send(200, "application/json", TIMENOW); 
}

void sendPowerStat(){
  String p1 = String(digitalRead(pinArray[0]));
  String p2 = String(digitalRead(pinArray[1]));
  String p3 = String(digitalRead(pinArray[2]));
  String p4 = String(digitalRead(pinArray[3]));
  String p5 = String(digitalRead(pinArray[4]));
  String p6 = String(digitalRead(pinArray[5]));
  String POWERNOW;
  POWERNOW = "{\"P1\" : \"";
  POWERNOW += p1;
  POWERNOW += "\", ";
  POWERNOW += "\"P2\" : \"";
  POWERNOW += p2;
  POWERNOW += "\", ";
  POWERNOW += "\"P3\" : \"";
  POWERNOW += p3;
  POWERNOW += "\", ";
  POWERNOW += "\"P4\" : \"";
  POWERNOW += p4;
  POWERNOW += "\", ";
  POWERNOW += "\"P5\" : \"";
  POWERNOW += p5;
  POWERNOW += "\", ";
  POWERNOW += "\"P6\" : \"";
  POWERNOW += p6;
  POWERNOW += "\"}";

  server1.send(200, "application/json", POWERNOW); 
}


void handleSchedule()
{   String Sstemp;
    String Temp;
    String NTPUNIQUE;
    RtcDateTime now = Rtc.GetDateTime();
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPMD =  String(now.Day());
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = "0" + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = "0" + NPMM; }   
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;
  
  if (NPMM.toInt() % 5 == 0){
     saveStatus();    
  }
  
  if (NTPUNIQUE != SCHMUNIQUE ){
    if(SPIFFS.exists("/schedule.json")){
    
    File configFile = SPIFFS.open("/schedule.json", "r");
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
      Sstemp = (const char*)root["ST"];
    
      sensors.requestTemperatures();   
      Temp = String(sensors.getTempCByIndex(0));
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
     SCHMUNIQUE = NTPUNIQUE;
}
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
    }else {
      //turn off
      digitalWrite(pinArray[3], 0);
    } 
     
   } else {
     if(tempfm < tempF){
       //turn on
      digitalWrite(pinArray[3], 1);
     }else {
      // turn off
      digitalWrite(pinArray[3], 0);
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
void checkSchedule(String sch, String hh, String mn, String dy)
{  
if (hh == sch.substring(4,6) && mn == sch.substring(7,9) && (sch.substring(13).indexOf(dy) >= 0 || sch.substring(13).indexOf("7") >= 0)    ){
    
      if (sch.substring(10,12) != "--"){handleMilis( sch.substring(0,1), sch.substring(10,12));
    } else{
      
      digitalWrite(pinArray[ sch.substring(0,1).toInt() - 1 ], sch.substring(2,3).toInt());
      delay(200);
    }
    
       }

}
void saveStatus()
{  
 DynamicJsonBuffer jsonBuffer;
 JsonObject& power = jsonBuffer.createObject();
 power["P1"] = String(digitalRead(pinArray[0]));
 power["P2"] = String(digitalRead(pinArray[1]));
 power["P3"] = String(digitalRead(pinArray[2]));
 power["P4"] = String(digitalRead(pinArray[3]));
 power["P5"] = String(digitalRead(pinArray[4]));
 power["P6"] = String(digitalRead(pinArray[5]));
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