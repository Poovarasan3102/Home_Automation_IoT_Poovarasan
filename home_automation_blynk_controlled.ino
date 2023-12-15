/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL-EVsksxa"
#define BLYNK_DEVICE_NAME "Home automation"
#define BLYNK_AUTH_TOKEN "LWDALlxholKb0qPOJSSSn82E0voGf7_D"


// Comment this out to disable prints 
//#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw,inlet_sw,outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;  //Blynk.virtualWrite(); setInterval(1000,);

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
  int value = param.asInt();
  if(value)
  {
    cooler_control(ON);
    lcd.setCursor(7,0);
    lcd.print("CO_LR ON ");
   
  }
  else{
   cooler_control(OFF); 
   lcd.setCursor(7,0);
    lcd.print("CO_LR OFF");   
  }
  
  
}

/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN )
{
  heater_sw = param.asInt();
  if(heater_sw)
  {
    heater_control(ON);
    lcd.setCursor(7,0);
    lcd.print("HE_TR ON ");
   
  }
  else{
   heater_control(OFF); 
   lcd.setCursor(7,0);
   lcd.print("HE_TR OFF");   
  }
}
/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  inlet_sw = param.asInt();
  if(inlet_sw)
  {
    enable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL ON ");
  }
  else
  {
    disable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL OFF");
  }
}
/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  if(outlet_sw)
  {
    enable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL ON ");
  }
  else
  {
    disable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL OFF");
  }
}
/* To display temperature and water volume as gauge on the Blynk App*/  
void update_temperature_reading()
{
  // You can send any value at any time.
  Blynk.virtualWrite(TEMPERATURE_GAUGE,read_temperature());  //sending temperature reading to temperature gauge for every one second 
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(WATER_VOL_GAUGE,volume()); //sending volume of water in the tank for every 1 second 
  
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
  if((read_temperature()>float(35)) && heater_sw)
  {
    heater_sw = 0;
    heater_control(OFF);
    lcd.setCursor(7,0);
    lcd.print("HE_TR OFF");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Temperature is above 35 degree Celcius\n");
    Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Turning OFF the Heater\n");
    Blynk.virtualWrite(HEATER_V_PIN, 0);
  }
}

/*To control water volume above 2000ltrs*/
void handle_tank(void)
{
 //vol < 2000 and inlet valve off
 if((tank_volume < 2000)&&(inlet_sw == 0))
 {
   enable_inlet();
   inlet_sw = 1;
   //To print notification on dashboard
   lcd.setCursor(7,1);
   lcd.print("IN_FL ON ");
   //To print notification on Blynk IoT app
   Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water level is less than 2000 litres\n");
   Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water Inflow enabled\n");
   Blynk.virtualWrite(INLET_V_PIN, 1); //reflect the status ON on the widget button of Inlet valve
   
 }
  //if volume is 3000 ltrs and if inlet valve is ON disable Inflow
  if((tank_volume == 3000)&&(inlet_sw == 1))
  {
   disable_inlet();
   inlet_sw = 0;
   lcd.setCursor(7,1);
   lcd.print("IN_FL OFF");
   Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water level is Full\n");
   Blynk.virtualWrite(BLYNK_TERMINAL_V_PIN, "Water Inflow disabled\n");
   Blynk.virtualWrite(INLET_V_PIN, 0);
  }

}


void setup(void)
{
    Blynk.begin(auth);
    lcd.init();                     
    lcd.backlight();
    lcd.clear();
    lcd.home();
    lcd.setCursor(0,0);
    lcd.print("T=");
    //To display volume of water
    lcd.setCursor(0,1);
    lcd.print("V=");
    init_temperature_system();
    init_ldr();
    init_serial_tank();
    //update temperature to Blynk app for every one second 
    timer.setInterval(1000L,update_temperature_reading);
}

void loop(void) 
{
    Blynk.run();  
    timer.run();
    String temperature;//read temperature and display it on dashboard
    temperature = String(read_temperature(),0); //27.37 =27
    lcd.setCursor(2,0);
    lcd.print(temperature);
    tank_volume = volume();
    lcd.setCursor(2,1);
    lcd.print(tank_volume);
    //To control the garden lights
    brightness_control();
    handle_temp(); //To control threshold temperature 35 degree
    handle_tank();
    
}

/*    Serial_write(0x00); 
      Serial_write(0x01);
      Serial_write(0x30); 
      valh=Serial_read(0); //Higher byte MSB
      vall=Serial_read(0); //lower byte LSB
      valor=(valh<<8)|vall;*/
