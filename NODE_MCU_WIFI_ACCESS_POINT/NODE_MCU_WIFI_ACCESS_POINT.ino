#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiGratuitous.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>
#include <WiFiServerSecureAxTLS.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiUdp.h>

/*********
 This code is from Rui Santos ,i do some changes to work this code for 4 ch. relay module 
 
  Complete project details at https://randomnerdtutorials.com  
*********/
/****** This code is for esp32 for using esp8266 just replace <wifi.h> into <esp8266wifi.h> , also change the  output GPIO pins ****/
// Load Wi-Fi library


// Replace with your network credentials
// to create its own wifi network
const char* ssid     = "ESP8266 Remote Control";
const char* password = "Paradise";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
//this pins for esp32 
//change the pins for esp8266
//String output12State = "off";
//String output13State = "off";
//String output14State = "off";
//String output15State = "off";
//String output16State = "off";
//String output5State = "off";
//String output4State = "off";
//String output0State = "off";
//String output2State = "off";

// Assign output variables to GPIO pins
const int output16 = 16; // for D0 of Node MCU
const int output5 =   5; // for D1 of Node MCU
const int output4 =   4; // for D2 of Node MCU
const int output0 =   0; // for D3 of Node MCU
const int output2 =   2; // for D4 of Node MCU
const int output14 = 14; // for D5 of Node MCU
const int output12 = 12; // for D6 of Node MCU
const int output13 = 13; // for D7 of Node MCU
const int output15 = 15; // for D8 of Node MCU

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output16, OUTPUT);
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(output0, OUTPUT);
  pinMode(output2, OUTPUT);
  pinMode(output14, OUTPUT);
  pinMode(output12, OUTPUT);
  pinMode(output13, OUTPUT);
  pinMode(output15, OUTPUT);
 
  // Set outputs to high because we are using active low type relay module
  digitalWrite(output16, LOW);
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
  digitalWrite(output0, LOW);
  digitalWrite(output2, LOW);
  digitalWrite(output14, LOW);
  digitalWrite(output12, LOW);
  digitalWrite(output13, LOW);
  digitalWrite(output15, LOW);


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
       // Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
           // client.println("HTTP/1.1 200 OK");
           // client.println("Content-type:text/html");
           // client.println("Connection: close");
           // client.println();
            
            // turns the GPIOs on and off
       if (header.indexOf("GET /16/on") >= 0)
            {
              Serial.println("SWITCH1 on");
             // output16State = "on";
              digitalWrite(output16, LOW);
              break;
            } 
            else if (header.indexOf("GET /16/off") >= 0)
            {
              Serial.println("SWITCH1 off");
            //  output16State = "off";
              digitalWrite(output16, HIGH);
              break;
            } 
      
      else if (header.indexOf("GET /5/on") >= 0)
            {
              Serial.println("SWITCH2 on");
             // output5State = "on";
              digitalWrite(output5, LOW);
              break;
            } 
            else if (header.indexOf("GET /5/off") >= 0)
            {
              Serial.println("SWITCH2 off");
            //  output5State = "off";
              digitalWrite(output5, HIGH);
              break;
            } 
      else if (header.indexOf("GET /4/on") >= 0)
            {
              Serial.println("SWITCH3 on");
            //  output4State = "on";
              digitalWrite(output4, LOW);
              break;
            } 
            else if (header.indexOf("GET /4/off") >= 0)
            {
              Serial.println("SWITCH3 off");
            //  output4State = "off";
              digitalWrite(output4, HIGH);
              break;
            } 
      else if (header.indexOf("GET /0/on") >= 0)
            {
              Serial.println("SWITCH4 on");
           //   output0State = "on";
              digitalWrite(output0, LOW);
              break;
            } 
            else if (header.indexOf("GET /0/off") >= 0)
            {
              Serial.println("SWITCH4 off");
           //   output0State = "off";
              digitalWrite(output0, HIGH);
              break;
            } 

      else if (header.indexOf("GET /2/on") >= 0)
            {
              Serial.println("SWITCH5 on");
            //  output2State = "on";
              digitalWrite(output2, LOW);
              break;
            } 
            else if (header.indexOf("GET /2/off") >= 0)
            {
              Serial.println("SWITCH5 off");
             // output2State = "off";
              digitalWrite(output2, HIGH);
              break;
            } 
             //for GPIO14
             else if (header.indexOf("GET /14/on") >= 0)
            {
              Serial.println("SWITCH6 on");
            //  output14State = "on";
              digitalWrite(output14, LOW);
              break;
            } 
            else if (header.indexOf("GET /14/off") >= 0)
            {
              Serial.println("SWITCH6 off");
            //  output14State = "off";
              digitalWrite(output14, HIGH);
              break;
            }     
      
            //foe GPIO12
            if (header.indexOf("GET /12/on") >= 0) 
            {
              Serial.println("SWITCH7 on");
            //  output12State = "on";
              digitalWrite(output12, LOW);
              break;
            } 
            else if (header.indexOf("GET /12/off") >= 0) 
            {
              Serial.println("SWITCH7 off");
           //   output12State = "off";
              digitalWrite(output12, HIGH);
              break;
            }
            //for GPIO13
            else if (header.indexOf("GET /13/on") >= 0)
            {
              Serial.println("SWITCH8 on");
           //   output13State = "on";
              digitalWrite(output13, LOW);
              break;
            } 
            else if (header.indexOf("GET /13/off") >= 0)
            {
              Serial.println("SWITCH8 off");
        //      output13State = "off";
              digitalWrite(output13, HIGH);
              break;
            }            
             //for GPIO15
            else if (header.indexOf("GET /15/on") >= 0)
            {
              Serial.println("SWITCH9 on");
            //  output15State = "on";
              digitalWrite(output15, LOW);
              break;
            } 
            else if (header.indexOf("GET /15/off") >= 0)
            {
              Serial.println("SWITCH9 off");
             // output15State = "off";
              digitalWrite(output15, HIGH);
              break;
            }        
            
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
