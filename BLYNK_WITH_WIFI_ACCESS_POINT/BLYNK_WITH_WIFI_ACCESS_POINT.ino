/*------------------------------------------------------------------------------
  10/2/2018
  Author: Makerbro
  Platforms: ESP8266
  Language: C++/Arduino
  File: wifi_modes_switch.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how  to switch the ESP8266 between Station 
  (STA) and Access Point (AP) modes, and how to store network credentials in 
  Flash memory:
  https://youtu.be/lyoBWH92svk
  Do you like my videos? You can support the channel:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  https://amazon.com/acrobotic
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <FS.h>
#include <ArduinoJson.h>

ESP8266WebServer server;
uint8_t pin_led = 4;
uint8_t pin_ledt = 5;
char* ssid = "YOUR_SSID"; //not used
char* password = "123456789";
char* mySsid = "ESP8266";
//char autho[] = "2hBSoxgyU9lwf5AncFJm7c0qurpGkmub";
//char ssido[] = "TP-LINK-191";
//char passo[] = "Paradise";

//IPAddress local_ip(192,168,11,4);
//IPAddress gateway(192,168,11,1);
//IPAddress netmask(255,255,255,0);

char webpage[] PROGMEM = R"=====(
<html>
<head>
</head>
<body>
  <form>
    <fieldset>
      <div>
        <label for="ssid">SSID</label>      
        <input value="" id="ssid" placeholder="SSID">
      </div>
      <div>
        <label for="password">PASSWORD</label>
        <input value="" id="password" placeholder="PASSWORD">
      </div>
    <div>
        <label for="auth">Auth Token</label>      
        <input value="" id="auth" placeholder="AUTH">
      </div>
      <div>
        <button class="primary" id="savebtn" type="button" onclick="myFunction()">SAVE</button>
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
  var auth = document.getElementById("auth").value;
  var data = {ssid:ssid, password:password, auth:auth};
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
  pinMode(pin_led, OUTPUT);
  pinMode(pin_ledt, INPUT);
  digitalWrite(pin_ledt,LOW);
  
  Serial.begin(115200);
  SPIFFS.begin();

  wifiConnect();

 // server.on("/",[](){server.send_P(200,"text/html", webpage);});
 // server.on("/toggle",toggleLED);
//  server.on("/settings", HTTP_POST, handleSettingsUpdate);
  
//  server.begin();
}

void loop()
{
   if ( digitalRead(pin_ledt) == LOW ) {
  
  Serial.println("Turning On AP");
  resetToAP ();
  while (1 != 2) 
        {
           server.handleClient();
           digitalWrite(pin_led,LOW);
           delay(1000);
           digitalWrite(pin_led,HIGH);
        }
 
 } 
    if (WiFi.status() == WL_CONNECTED)
  {
      
 digitalWrite(pin_led,LOW);
  Blynk.run();

  }
 
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
  
  wifiConnect();
}

void wifiConnect()
{
  //reset networking
  WiFi.softAPdisconnect(true);
  WiFi.disconnect();          
  delay(1000);
  //check for stored credentials
  if(SPIFFS.exists("/config.json")){
    const char * _ssid = "", *_pass = "", *_auth = "";
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
        _auth = jObject["auth"];
    Serial.println(_auth);
       // WiFi.mode(WIFI_STA);
        //WiFi.begin(_ssid, _pass);
        Blynk.begin(_auth, _ssid, _pass);
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

void toggleLED()
{
  digitalWrite(pin_led,!digitalRead(pin_led));
  server.send(204,"");
}

void resetToAP ()
{   WiFi.softAPdisconnect(true);
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
    server.on("/toggle",toggleLED);
    server.on("/settings", HTTP_POST, handleSettingsUpdate);
    server.begin(); 
delay(2000);  
    
}
