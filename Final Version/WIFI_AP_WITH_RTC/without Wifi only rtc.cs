#include <FS.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RtcDS3231.h>

int pinArray[] = {16,0,14,12,13,15}; //D0,D3,D5,D6,D7,D8

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
  digitalWrite(pinArray[1], 1);
  digitalWrite(pinArray[2], 1);
  
  SPIFFS.begin();
  Rtc.Begin();
  //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  //Rtc.SetDateTime(compiled);
//  syncStatus();
  
}

void loop(){
  //handleScheduleUpdate();
  handleSchedule(); 
  sendTime();
  delay(1000);
}

void handleScheduleUpdate()
{ 
  String data ="{\"S01\" : \"5:0:22:00:--:7\",\"S02\" : \"5:1:17:00:--:7\",\"S03\" : \"6:0:07:00:--:7\",\"S04\" : \"6:1:22:00:--:7\",\"S05\" : \"1:0:22:00:--:7\",\"S06\" : \"1:1:07:00:--:7\",\"S07\" : \"4:1:12:00:--:7\",\"S08\" : \"4:0:17:00:--:7\",\"S09\" : \"\",\"S10\" : \"\",\"S11\" : \"\",\"S12\" : \"\",\"ST\" : \"26.00\"}";
  DynamicJsonBuffer jBuffer;
  JsonObject& jObject = jBuffer.parseObject(data); 
  File configFile = SPIFFS.open("/schedule.json", "w");
  jObject.printTo(configFile);  
  configFile.close();
  Serial.print("Saved");
}



/* void setRtcDateTime()
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
 */
void sendTime(){
        String Temp;
    String TIMENOW;
    String DTTIME;
//  Temp = String(sensors.getTempCByIndex(0));
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
  
  Serial.print(TIMENOW);
 // server1.send(200, "application/json", TIMENOW); 
}


void handleSchedule()
{   
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
  
/*   if (NPMM.toInt() % 5 == 0){
     saveStatus();    
  } */
  
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
String schList [12] = {Sch1,Sch2,Sch3,Sch4,Sch5,Sch6,Sch7,Sch8,Sch9,Sch10,Sch11,Sch12};
checkScheduleNew(schList, NPMH, NPMM,NPWD);
   /*  checkSchedule(Sch1, NPMH, NPMM,NPWD );
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
    checkSchedule(Sch12, NPMH, NPMM,NPWD ); */
       
        
    } 
  }
}
     SCHMUNIQUE = NTPUNIQUE;
}
}


/* void checkSchedule(String sch, String hh, String mn, String dy)
{  
if (hh == sch.substring(4,6) && mn == sch.substring(7,9) && (sch.substring(13).indexOf(dy) >= 0 || sch.substring(13).indexOf("7") >= 0)    ){    
      digitalWrite(pinArray[ sch.substring(0,1).toInt() - 1 ], sch.substring(2,3).toInt());
      delay(200);    
       }

} */
/* void saveStatus()
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
} */
/* void syncStatus()
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
} */

void checkScheduleNew(String scharr[], String hh, String mn, String dy) {
  int schintarr [12][4];
  String schtimechar;
  String currtimechar;
  int  currtimeint;
  int  prevscheduleint;
  int  power [6]={-1,-1,-1,-1,-1,-1};
  currtimechar = hh;
  currtimechar += mn;
  currtimeint = currtimechar.toInt();
  for (int i = 0; i < 12; i++) {
  schtimechar = scharr[i].substring(4, 6) + scharr[i].substring(7, 9); //+ scharr[i].substring(10, 12);--hhmiss
    schintarr[i][0] = scharr[i].substring(0, 1).toInt();   //powerpoint
    schintarr[i][1] = scharr[i].substring(2, 3).toInt();   // state
    schintarr[i][2] = schtimechar.toInt();
    if (scharr[i].substring(13).indexOf(dy) >= 0 || scharr[i].substring(13).indexOf("7") >= 0) {
      schintarr[i][3] = 1;     //Active Day
    } else {
      schintarr[i][3] = 0;     //Inactive Day
    }
  }
  
  for (int p = 0; p < 6; p++) {//for each powerpoint
      prevscheduleint = 0;
    for (int i = 0; i < 12; i++) {//for each schedule     
      if (schintarr[i][3] == 1 && schintarr[i][0] == (p + 1))  {
        if (currtimeint >= schintarr[i][2] &&  schintarr[i][2] > prevscheduleint) {
          power[p] = schintarr[i][1];
      prevscheduleint = schintarr[i][2];
        }
    //    if (i != 0) {
        //  prevscheduleint = schintarr[i][2];
//}
      }
    
    }
  }
  for (int p = 0; p < 6; p++) {
    //for each powerpoint   
    if( power[p] >= 0){
    digitalWrite(pinArray[p], power[p]);
    delay(200);
    }

  }
}