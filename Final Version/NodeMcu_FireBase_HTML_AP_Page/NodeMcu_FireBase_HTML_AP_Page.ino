#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

//1. Change the following info
#define FIREBASE_HOST "esp8266-36d42.firebaseio.com"
#define FIREBASE_AUTH "RiqMLVZkq4yeWS4QUScQyo6Da3vQiBaan49UhvVF"
//#define WIFI_SSID "TP-LINK-191"
//#define WIFI_PASSWORD "Paradise" 
String user = String( "/USER2"); 
String firebasePath= String ("/LED_");
ESP8266WebServer server;
//uint8_t pin_led = 2;
//uint8_t pin_ledt = 14;
char* ssid = "YOUR_SSID"; //not used
char* password = "123456789";
char* mySsid = "ESP8266";

int timezone = 19800;//5 * 3600;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//2. Define FirebaseESP8266 data object for data sending and receiving
FirebaseData firebaseData;
// Assign output variables to GPIO pins
const int OP1 = 16; // for D0 of Node MCU
const int OP2 =   5; // for D1 of Node MCU
const int OP3 =   4; // for D2 of Node MCU
const int OP4 =   0; // for D3 of Node MCU
const int pin_led = 2;
const int pin_ledt = 14;
const String add0 = "0";
int pinArray[] = {16,5,4,0};
String S1;
int S1Si ;
String S1UNIQUE;
String S2UNIQUE;
String S3UNIQUE;
String S4UNIQUE;
String S5UNIQUE;
String S6UNIQUE;
String S7UNIQUE;
String S8UNIQUE;
String NTPUNIQUE;
String NPMH;
String NPMM;
String NPMD;
String firebasefPath;

char webpage[] PROGMEM = R"=====(
<html>
<head>
</head>
<body>
  <form>
    <fieldset>
      <div>
        <label for="ssid">WiFi SSID</label>      
        <input value="" id="ssid" placeholder="SSID">
      </div>
      <div>
        <label for="password">WiFi PASSWORD</label>
        <input value="" id="password" placeholder="PASSWORD">
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
  var ssid = document.getElementById("ssid").value;
  var password = document.getElementById("password").value;
  var data = {ssid:ssid, password:password};
  var xhr = new XMLHttpRequest();
  var url = "/settings";
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


void setup()
{

 Serial.begin(115200);
  pinMode(OP1, OUTPUT);
  pinMode(OP2, OUTPUT);
  pinMode(OP3, OUTPUT);
  pinMode(OP4, OUTPUT);
  pinMode(pin_ledt, INPUT);
  pinMode(pin_led, OUTPUT);
  SPIFFS.begin();
//  digitalWrite(OP1, LOW);
//  digitalWrite(OP2, LOW);
//  digitalWrite(OP3, LOW);
//  digitalWrite(OP4, LOW);
digitalWrite(pin_ledt,LOW);

wifiConnect();

 // Serial.println();
 // Serial.print("Connected with IP: ");
 // Serial.println(WiFi.localIP());
 // Serial.println();
 // configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
 // Serial.println("\nWaiting for Internet time");

//  while(!time(nullptr)){
//     Serial.print("*");
//     delay(5000);
//  }
//  Serial.println("\nTime response....OK");   
  timeClient.begin();
  timeClient.setTimeOffset(timezone);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

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
 checkSchedule();
 doManualTask(); 
 }
}
void checkSchedule()
{ 
 timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm *p_tm = gmtime ((time_t *)&epochTime); 
 // struct tm* p_tm = localtime(&now);
    NPMH = p_tm->tm_hour ;
    NPMM = p_tm->tm_min;
    NPMD = p_tm->tm_mday;
    if (NPMH.length() < 2 ) {NPMH = add0 + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = add0 + NPMM; }
    NTPUNIQUE = NPMD + NPMH;
    NTPUNIQUE += NPMM;
   if(Firebase.getString(firebaseData, user +"/SCH/S1"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S1UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S1UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S1UNIQUE = NTPUNIQUE;
    }
  }
  }
  
  if(Firebase.getString(firebaseData,user +"/SCH/S2"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S2UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S2UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S2UNIQUE = NTPUNIQUE;
    }
  }
  }


  
    if(Firebase.getString(firebaseData, user +"/SCH/S3"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S3UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S3UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S3UNIQUE = NTPUNIQUE;
    }
  }
  }


  
    if(Firebase.getString(firebaseData, user +"/SCH/S4"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S4UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S4UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S4UNIQUE = NTPUNIQUE;
    }
  }
  }


  
    if(Firebase.getString(firebaseData, user +"/SCH/S5"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S5UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S5UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S5UNIQUE = NTPUNIQUE;
    }
  }
  }


  
    if(Firebase.getString(firebaseData, user +"/SCH/S6"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S6UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S6UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S6UNIQUE = NTPUNIQUE; 
    }
  }
  }

  
    if(Firebase.getString(firebaseData,user +"/SCH/S7"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
    if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S7UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S7UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S7UNIQUE = NTPUNIQUE;
    }
  }
  }


  
    if(Firebase.getString(firebaseData, user +"/SCH/S8"))
  {
    S1 = firebaseData.stringData();
    S1Si = S1.substring(0,1).toInt();
    firebasefPath = firebasePath + S1.substring(0,1);
  
  if (NPMH == S1.substring(4,6) && NPMM == S1.substring(7,9) && NTPUNIQUE != S8UNIQUE  ){
    if (S1.substring(2,3) =="F"){
      digitalWrite(pinArray[ S1Si - 1 ], LOW);
      Firebase.setString(firebaseData, firebasefPath, "F");
      S8UNIQUE = NTPUNIQUE;
    } else if (S1.substring(2,3) =="O"){
      digitalWrite(pinArray[S1Si - 1], HIGH);
      Firebase.setString(firebaseData,firebasefPath, "ON");
      S8UNIQUE = NTPUNIQUE; 
    }
  }
  
  }

}

void doManualTask()
{ 
  if(Firebase.getString(firebaseData, user +"/LED_1"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(16, HIGH);
  } else
  { 
        digitalWrite(16, LOW);
  }
  }
  
     if(Firebase.getString(firebaseData, user + "/LED_2"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(5, HIGH);
  } else
  { 
        digitalWrite(5, LOW);
  }
  }
  
     if(Firebase.getString(firebaseData, user + "/LED_3"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(4, HIGH);
  } else
  { 
        digitalWrite(4, LOW);
  }
  }
  
     if(Firebase.getString(firebaseData, user + "/LED_4"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(0, HIGH);
  } else
  { 
        digitalWrite(0, LOW);
  }
  }
  
}

void startupSync()
{ 
//Sync LED1
  if(Firebase.getString(firebaseData, user + "/LED_1"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(16, HIGH);
  } else
  { 
        digitalWrite(16, LOW);
  }
  } 
  //If not present then create
  else {
    Firebase.setString(firebaseData, user + "/LED_1", "F");
  }
  
// Sync LED 2
     if(Firebase.getString(firebaseData, user + "/LED_2"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(5, HIGH);
  } else
  { 
        digitalWrite(5, LOW);
  }
  }
// If not present then create
  else {
    Firebase.setString(firebaseData, user + "/LED_2", "F");
  }
  
 // Sync LED 3 
     if(Firebase.getString(firebaseData, user + "/LED_3"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(4, HIGH);
  } else
  { 
        digitalWrite(4, LOW);
  }
  }
 // If not present then create
  else {
    Firebase.setString(firebaseData, user + "/LED_3", "F");
  }
  
  //Sync LED 4
     if(Firebase.getString(firebaseData, user + "/LED_4"))
  {
   if (firebaseData.stringData()== "ON") {
    digitalWrite(0, HIGH);
  } else
  { 
        digitalWrite(0, LOW);
  }
  }
  
  // If not present then create
  else {
    Firebase.setString(firebaseData, user + "/LED_4", "F");
  }
  
}
void syncScheduleNodes()
{ 

  if(!Firebase.getString(firebaseData, user + "/SCH/S1"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S1", "");
  } 
 
   if(!Firebase.getString(firebaseData, user + "/SCH/S2"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S2", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/SCH/S3"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S3", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/SCH/S4"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S4", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/SCH/S5"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S5", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/SCH/S6"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S6", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/SCH/S7"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S7", "");
  } 
    if(!Firebase.getString(firebaseData, user + "/SCH/S8"))
  {
   Firebase.setString(firebaseData, user + "/SCH/S8", "");
  } 
  
}
void wifiConnect()
{
  //reset networking
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();          
  delay(1000);
  //check for stored credentials
  if(SPIFFS.exists("/config.json")){
    const char * _ssid = "", *_pass = "";
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
  
  server.send(200, "application/json", "{\"status\" : \"ok\"}");
  delay(2000);
  
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
    delay(5000);
    server.on("/",[](){server.send_P(200,"text/html", webpage);});
    server.on("/settings", HTTP_POST, handleSettingsUpdate);
    server.begin(); 
    delay(2000);  
    while (1 != 2) 
        {
           server.handleClient();
digitalWrite(pin_led,!digitalRead(pin_led));
delay(500);
        }
    
}
