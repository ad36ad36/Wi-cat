//www.elegoo.com [2016.12.08]
// Modified for Adafruit HUZZAH32 ESP32 Feather (5/2/2019)

const int buzzer = 16; // GPIO pin for active buzzer

void setup()
{
  pinMode(buzzer,OUTPUT); //initialize the buzzer pin as an output
}

void loop()
{
  unsigned char i;
  while(1)
  {
    //output a frequency
    for(i=0;i<80;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(1);//wait for 1ms
      digitalWrite(buzzer,LOW);
      delay(1);//wait for 1ms
    }

    //output another frequency
    for(i=0;i<100;i++)
    {
      digitalWrite(buzzer,HIGH);
      delay(2);//wait for 2ms
      digitalWrite(buzzer,LOW);
      delay(2);//wait for 2ms
    }
  }
