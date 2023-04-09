#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>


String user = String( "/AqLxuDSN7"); 
String firebasePath= String ("/P_");
ESP8266WebServer server;
char* mySsid   = "AquaLink";
char* password = "123456789";
int timezone = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
FirebaseData firebaseData;

const int OP1 = 16; //  D0 
const int OP2 =   5; // D1
const int OP3 =   4; // D2 
const int OP4 =   0; // D3
const int OP5 =   12; //D6
const int OP6 =   13; //D7
const int pin_led = 2;//D4
const int pin_ledt = 14; // D5
const String add0 = "0";
int pinArray[] = {16,5,4,0,12,13};

String FBLOSTA;
String FBuQ;
String S1UNIQUE;
String S2UNIQUE;
String S3UNIQUE;
String S4UNIQUE;
String S5UNIQUE;
String S6UNIQUE;
String S7UNIQUE;
String S8UNIQUE;
String S9UNIQUE;
String S10UNIQUE;
String S11UNIQUE;
String S12UNIQUE;
String NTPUNIQUE;
//String NPMH;
//String NPMMON;
//String NPMYR;
//String NPMM;
//String NPMD;
//String NPWD;
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
</script>
</html>
)=====";


void setup()
{

 Serial.begin(115200);
  pinMode(OP1, OUTPUT);
  pinMode(OP2, OUTPUT);
  pinMode(OP3, OUTPUT);
  pinMode(OP4, OUTPUT);
  pinMode(OP5, OUTPUT);
  pinMode(OP6, OUTPUT);
  pinMode(pin_ledt, INPUT);
  pinMode(pin_led, OUTPUT);
  SPIFFS.begin();
digitalWrite(pin_ledt,LOW);

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

if ( digitalRead(pin_ledt) == HIGH ) {
   resetToAP ();
   
 } else {  
 if (WiFi.status() == WL_CONNECTED)
  {
digitalWrite(pin_led,HIGH);
  } else {digitalWrite(pin_led,LOW);}
if(Firebase.get(firebaseData, user))
    {
  doManualTask(firebaseData); 
  checkSchedule(firebaseData);
 }
 }
 
}

void checkSchedule (FirebaseData &data)
{ 
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
		  
	FBLOSTA = NPMD + ":";
	FBLOSTA += NPMMON;
	FBLOSTA += ":";
	FBLOSTA += NPMYR;
	FBLOSTA += ":";
	FBLOSTA += NPMH;
	FBLOSTA += ":";
	FBLOSTA += NPMM;     

	 FirebaseJson &json = data.jsonObject();
     String jsonStr;
     json.toString(jsonStr,true);
     DynamicJsonBuffer jsonBuffer;
     JsonObject& root = jsonBuffer.parseObject(jsonStr);
      String Sch1 = root["S01"]; 
      String Sch2 = root["S02"];  
      String Sch3 = root["S03"]; 
      String Sch4 = root["S04"]; 
      String Sch5 = root["S05"]; 
      String Sch6 = root["S06"]; 
      String Sch7 = root["S07"]; 
      String Sch8 = root["S08"]; 
	  String Sch9 = root["S09"];
	  String Sch10 = root["S10"];
	  String Sch11 = root["S11"];
	  String Sch12 = root["S12"];  
	  Serial.print(Sch1);
	  
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
	  }
}

void doManualTask(FirebaseData &data)
{ 
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
    
      digitalWrite(16, Led1.toInt());
      digitalWrite(5, Led2.toInt());
      digitalWrite(4, Led3.toInt());
      digitalWrite(0, Led4.toInt());
	  digitalWrite(12, Led5.toInt());
	  digitalWrite(13, Led6.toInt());

     
}

void startupSync()
{ 


  if(Firebase.getString(firebaseData, user + "/P_1"))
  {
    digitalWrite(16, firebaseData.stringData().toInt());
  } 
  else {
    Firebase.setString(firebaseData, user + "/P_1", "0");
  }
  
     if(Firebase.getString(firebaseData, user + "/P_2"))
  {
    digitalWrite(5, firebaseData.stringData().toInt());
  }

  else {
    Firebase.setString(firebaseData, user + "/P_2", "0");
  }
  

     if(Firebase.getString(firebaseData, user + "/P_3"))
  {
    digitalWrite(4, firebaseData.stringData().toInt());
  }

  else {
    Firebase.setString(firebaseData, user + "/P_3", "0");
  }
  

     if(Firebase.getString(firebaseData, user + "/P_4"))
  {
    digitalWrite(0, firebaseData.stringData().toInt());

  }
  

  else {
    Firebase.setString(firebaseData, user + "/P_4", "0");
  }

     if(Firebase.getString(firebaseData, user + "/P_5"))
  {
    digitalWrite(12, firebaseData.stringData().toInt());

  }
  

  else {
    Firebase.setString(firebaseData, user + "/P_5", "0");
  }

     if(Firebase.getString(firebaseData, user + "/P_6"))
  {
    digitalWrite(13, firebaseData.stringData().toInt());

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
          if ((unsigned long)(millis() - startTime) >= 5000) break;
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
    server.begin(); 
    delay(1000);  
    while (1 != 2) 
        {
           server.handleClient();
               digitalWrite(pin_led,!digitalRead(pin_led));
             delay(200);
        }
    
}
