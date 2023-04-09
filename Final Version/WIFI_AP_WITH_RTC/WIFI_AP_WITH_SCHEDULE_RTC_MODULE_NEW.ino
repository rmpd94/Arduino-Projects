#include <ESP8266WebServer.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RtcDS3231.h>

const char* ssid     = "ESP8266_D001";
const char* password = "Paradise";
String deviceId     = String( "/xyZkml");

String setngPath     = String( "/schedules");
String adhocOpPath   = String( "/operations");
String timeNow       = String( "/timenow");
String powerStat     = String( "/powerstat");
String setDateTime   = String( "/setdatetime");
int pinArray[] = {14,12,13,15};
ESP8266WebServer server1;
RtcDS3231<TwoWire> Rtc(Wire);
const int output16 = 16; // for D0 of Node MCU
//const int output5 =   5; // for D1 of Node MCU  -- connect SDA to D2 (GPIO 4)
                           //                                SCL to D1 (GPIO 5) in RTC Module
//const int output4 =   4; // for D2 of Node MCU
const int output0 =   0; // for D3 of Node MCU
const int output2 =   2; // for D4 of Node MCU
const int output14 = 14; // for D5 of Node MCU
const int output12 = 12; // for D6 of Node MCU
const int output13 = 13; // for D7 of Node MCU
const int output15 = 15; // for D8 of Node MCU

String SCHMUNIQUE;
String NTPUNIQUE;
String DTTIME;
String TIMENOW;
String POWERNOW;


void setup() {
  Serial.begin(115200);
  pinMode(output16, OUTPUT);
 // pinMode(output5, OUTPUT);
 // pinMode(output4, OUTPUT);
  pinMode(output0,OUTPUT);
  pinMode(output2,OUTPUT);
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
  //Rtc.SetDateTime(RtcDateTime("Sep 09 2020", "22:20:00")); // Aprox 45 seconds to upload the code. use this to set time.. then comment and re upload
   // Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__)); //to set date from system
   // Rtc.Enable32kHzPin(false);
   // Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);
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

  
}

void loop(){
  server1.handleClient(); 
  handleSchedule(); 
}

void handleScheduleUpdate()
{
  String data = server1.arg("plain");
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data);

  File configFile = SPIFFS.open("/config.json", "w");
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
     digitalWrite(power.toInt(), operation.toInt());
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
      
  RtcDateTime rtctm = Rtc.GetDateTime();
  String  ADNPMH =  String(rtctm.Hour());
  String  ADNPMM =  String(rtctm.Minute());
  String  ADNPMD =  String(rtctm.Day());
  String  ADNPMN =  String(rtctm.Month());
  String  ADNPMY =  String(rtctm.Year());

  if (ADNPMH.length() < 2 ) {ADNPMH = '0' + ADNPMH; }
  if (ADNPMM.length() < 2 ) {ADNPMM = '0' + ADNPMM; }
  if (ADNPMN.length() < 2 ) {ADNPMN = '0' + ADNPMN; }
  if (ADNPMD.length() < 2 ) {ADNPMD = '0' + ADNPMD; }

  DTTIME  = ADNPMD + ":";
  DTTIME += ADNPMN;
  DTTIME += ":";
  DTTIME += ADNPMY;
  DTTIME += ":";
  DTTIME += ADNPMH;
  DTTIME += ":";
  DTTIME += ADNPMM;
  TIMENOW = "{\"timeNow\" : \"";
  TIMENOW += DTTIME;
  TIMENOW += "\"}";
  //Serial.print(TIMENOW);
  server1.send(200, "application/json", TIMENOW); 
}

void sendPowerStat(){
  String p1 = String(digitalRead(pinArray[0]));
  String p2 = String(digitalRead(pinArray[1]));
  String p3 = String(digitalRead(pinArray[2]));
  String p4 = String(digitalRead(pinArray[3]));
  
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
  POWERNOW += "\"}";

  server1.send(200, "application/json", POWERNOW); 
}


void handleSchedule()
{
    RtcDateTime now = Rtc.GetDateTime();
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPMD =  String(now.Day());
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = '0' + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = '0' + NPMM; }   
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;
  if (NTPUNIQUE != SCHMUNIQUE ){
          SCHMUNIQUE = NTPUNIQUE;
     
    if(SPIFFS.exists("/config.json")){
    
    File configFile = SPIFFS.open("/config.json", "r");
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
