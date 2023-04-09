int pinData= 0;
unsigned long lengthHeader;
unsigned long bit;
int byteValue;
void setup()
{

pinMode(pinData, INPUT);
pinMode(1,OUTPUT);
pinMode(2,OUTPUT);
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


if(byteValue ==10)
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
  
  if(byteValue ==27)
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
     
byteValue=0;
delay(250);
}

