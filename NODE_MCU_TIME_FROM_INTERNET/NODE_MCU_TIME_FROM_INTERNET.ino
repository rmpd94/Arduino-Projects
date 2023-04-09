#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "TP-LINK-191";
const char* password = "Paradise";

int ledPin = 13;

int timezone = 23400;//5 * 3600;
int dst = 0;

void setup() {
  
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);

  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to ");
  Serial.println( ssid );

  WiFi.begin(ssid,password);

  Serial.println();
  
  Serial.print("Connecting");

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");        
  }

  
  Serial.println();

  Serial.println("Wifi Connected Success!");
  Serial.print("NodeMCU IP Address : ");
  Serial.println(WiFi.localIP() );

  configTime(timezone, dst, "pool.ntp.org","time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while(!time(nullptr)){
     Serial.print("*");
     delay(5000);
  }
  Serial.println("\nTime response....OK");   
}

void loop() {
  
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
   Serial.println( p_tm->tm_hour );
   Serial.println(p_tm->tm_min );
   Serial.println( p_tm->tm_sec );
  // TURN LED ON

  if( (p_tm->tm_hour == 8) && (p_tm->tm_min == 56)){
      digitalWrite(ledPin,HIGH);
  }
  

  // TURN LED OFF

  if( (p_tm->tm_hour == 8) && (p_tm->tm_min == 57)){
      digitalWrite(ledPin,LOW);
  }
  
  delay(1000);

  

}
