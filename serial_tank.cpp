#include "serial_tank.h"
#include "Arduino.h"
#include "main.h"

unsigned int volume_value;
unsigned char valueh, valuel;

void init_serial_tank(void) 
{
    Serial.begin(19200);
    Serial.write(0xFF); //synchronization
    Serial.write(0xFF);
    Serial.write(0xFF);   
}

unsigned int volume(void)
{
  Serial.write(VOLUME);
  //waiting for the serial data
  while(!Serial.available()); //true if any data is available(while loop get terminated) , false when data is not available(while loop keep on executing) 
   valueh=Serial.read(); //Higher byte MSB
  while(!Serial.available());
   valuel=Serial.read(); //lower byte LSB
   volume_value=(valueh<<8)|valuel; 
   return volume_value;
}
void enable_inlet(void)
{
    Serial.write(INLET_VALVE);
    Serial.write(ENABLE); 
}  
void disable_inlet(void)
{
    Serial.write(INLET_VALVE);
    Serial.write(DISABLE); 
}  
void enable_outlet(void)
{  
    Serial.write(OUTLET_VALVE);
    Serial.write(ENABLE); 
}
void disable_outlet(void)
{  
    Serial.write(OUTLET_VALVE);
    Serial.write(DISABLE);
}
