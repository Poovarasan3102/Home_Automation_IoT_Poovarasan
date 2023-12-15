#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
   pinMode(GARDEN_LIGHT, OUTPUT);
   
}
void brightness_control(void)
{
  //read the values from LDR sensor
  unsigned short inputVal;
  inputVal = analogRead(LDR_SENSOR);
  //scale it down from 0 to 1023  -  0 to 255
  int Voltage = map(inputVal, 0,1023, 255,0);
  //set the PWM from 255 to 0
  //Voltage = 255 - Voltage;
  analogWrite (GARDEN_LIGHT, Voltage);
  delay(500);
}
