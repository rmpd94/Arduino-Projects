/*
1= 126
2=246
3=946
4=846
5=286
6=906
7=666
8=826
9=746

*/


int brojac=0;
int pinData= 0;
unsigned long lengthHeader;
unsigned long bit;
int byteValue;
int vrime=70 ;
void setup()
{

pinMode(pinData, INPUT);
pinMode(1,OUTPUT);
pinMode(2,OUTPUT);
pinMode(3,OUTPUT);
pinMode(4,OUTPUT);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);
}

void loop()
{

lengthHeader = pulseIn(pinData, LOW);
if(lengthHeader > 1500)
{


for(int i = 1; i <= 32; i++)
{
bit = pulseIn(pinData, HIGH);


if (i > 16 && i <= 24)
if(bit > 1000)
byteValue = byteValue + (1 << (i - 17)); 
}
}


if(byteValue==12)
 
  {
    if(digitalRead(1)==LOW)
        {
        digitalWrite(1,HIGH);
       
        }
        else
        {
        digitalWrite(1,LOW);
        }
  }
 
  if(byteValue==24)
 
  {
    if(digitalRead(2)==LOW)
        {
        digitalWrite(2,HIGH);
        }
        else
        {
        digitalWrite(2,LOW);
        }
  
      }
      if(byteValue==94)
 
  {
    if(digitalRead(3)==LOW)
        {
        digitalWrite(3,HIGH);
        }
        else
        {
        digitalWrite(3,LOW);
        }
  
      }
      if(byteValue==8)
 
  {
    if(digitalRead(4)==LOW)
        {
        digitalWrite(4,HIGH);
        }
        else
        {
        digitalWrite(4,LOW);
        }
  
      }
      if(byteValue==69)
      {
        brojac=brojac+1;
        if(brojac==1)
        {
        digitalWrite(1,HIGH);
        digitalWrite(2,HIGH);
        digitalWrite(3,HIGH);
        digitalWrite(4,HIGH);
        }
        if(brojac==2)
        {
        digitalWrite(1,LOW);
        digitalWrite(2,LOW);
        digitalWrite(3,LOW);
        digitalWrite(4,LOW);
        brojac=0;
        }
      }
      
      if(byteValue==21 )
      {
      
        
digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);

digitalWrite(1,HIGH);
delay(vrime);
digitalWrite(1,LOW);

digitalWrite(2,HIGH);
delay(vrime);
digitalWrite(2,LOW);

digitalWrite(3,HIGH);
delay(vrime);
digitalWrite(3,LOW);

digitalWrite(4,HIGH);
delay(vrime);
digitalWrite(4,LOW);
        
        
        
      }
byteValue=0;
delay(250);
}

