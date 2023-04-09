#include <Wire.h>
#include <RtcDS3231.h>


RtcDS3231<TwoWire> Rtc(Wire);


void setup() {
Serial.begin(9600);
 
 Rtc.Begin();
// RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
 //Rtc.SetDateTime(compiled);

  
}

void loop(){
sendTime();
  delay(1000);
}

void sendTime(){
    String TIMENOW;
    String DTTIME;
  RtcDateTime rtctm = Rtc.GetDateTime();
  String  ADNPMH =  String(rtctm.Hour());
  String  ADNPMM =  String(rtctm.Minute());
  String  ADNPSS =  String(rtctm.Second());
  String  ADNPMD =  String(rtctm.Day());
  String  ADNPMN =  String(rtctm.Month());
  String  ADNPMY =  String(rtctm.Year());

  if (ADNPMH.length() < 2 ) {ADNPMH = "0" + ADNPMH; }
  if (ADNPMM.length() < 2 ) {ADNPMM = "0" + ADNPMM; }
  if (ADNPMN.length() < 2 ) {ADNPMN = "0" + ADNPMN; }
  if (ADNPMD.length() < 2 ) {ADNPMD = "0" + ADNPMD; }
  if (ADNPSS.length() < 2 ) {ADNPSS = "0" + ADNPSS; }
  

  DTTIME  = ADNPMD + ":";
  DTTIME += ADNPMN;
  DTTIME += ":";
  DTTIME += ADNPMY;
  DTTIME += ":";
  DTTIME += ADNPMH;
  DTTIME += ":";
  DTTIME += ADNPMM;
  DTTIME += ":";
  DTTIME += ADNPSS;
  TIMENOW  = "{\"timeNow\" : \"";
  TIMENOW += DTTIME;
  TIMENOW += "\"}";
  Serial.print(TIMENOW);
}
