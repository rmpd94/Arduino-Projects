#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <RtcDS1307.h>

#define user "/test3"


const char* FIREBASE_HOST = "esp8266-a194b.firebaseio.com"; 
const char* FIREBASE_AUTH = "IolC4yempcfksvOkrXQtoUQVF6Fwl7NTUtAYcf3R";
const char* WIFI_SSID = "TP-LINK-191";
const char* WIFI_PASSWORD = "Paradise";
FirebaseData firebaseData;
RtcDS1307<TwoWire> Rtc(Wire);
unsigned long PrevMillis = 0;

void setup()
{
  Serial.begin(9600);

 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
 Rtc.Begin();
 //RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
 //Rtc.SetDateTime(compiled);
    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
 Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 
}


void loop()
{  

if (millis() - PrevMillis > 6000 || PrevMillis == 0) {  
  PrevMillis = millis();
handleOffSch();
}
}

void handleOffSch()
{
     Serial.println("Enter ");
    RtcDateTime now = Rtc.GetDateTime();
    String NPMH =  String(now.Hour());
    String NPMM =  String(now.Minute());
    String NPSS =  String(now.Second());
    String NPMD =  String(now.Day());
    String NPMMON = String(now.Month());
    String NPMYR = String(now.Year());
    String NPWD =  String(now.DayOfWeek());
    if (NPMH.length() < 2 ) {NPMH = "0" + NPMH; }
    if (NPMM.length() < 2 ) {NPMM = "0" + NPMM; }
    if (NPMM.length() < 2 ) {NPSS = "0" + NPSS; }  
    if (NPMD.length() < 2 ) {NPMD = "0" + NPMD; }
    if (NPMMON.length() < 2 ) {NPMMON = "0" + NPMMON; } 
  String FBLOSTA = NPMD + "-";
  FBLOSTA += NPMMON;
  FBLOSTA += "-";
  FBLOSTA += NPMYR;
  FBLOSTA += ";";
  FBLOSTA += NPMH;
  FBLOSTA += ":";
  FBLOSTA += NPMM;
  FBLOSTA += ":";
  FBLOSTA += NPSS; 
  Firebase.setString(firebaseData, user "/LO", FBLOSTA); 
  Serial.println(FBLOSTA);
  Serial.println("exit ");
}