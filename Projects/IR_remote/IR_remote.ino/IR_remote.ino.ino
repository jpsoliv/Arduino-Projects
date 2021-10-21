#include <IRremote.h>
#include <IRremoteInt.h>

int RECV_PIN = 11;
int led_pin = 13;
int val = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
}

void loop()
{
  if (irrecv.decode(&results))
    {
     if (results.value == 0xCE0C6BA8)
     {
       val = digitalRead(led_pin);
       if (val == LOW)
         digitalWrite(led_pin,HIGH);
       else
         digitalWrite(led_pin,LOW);
      }
      
     Serial.println(results.value, HEX);
     irrecv.resume(); // Receive the next value
    }
}
