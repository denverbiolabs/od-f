#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd;

void setup(){
  Wire.begin();
  Serial.begin(9600);
}
void loop(){
  Serial.print("BOARD ID: 0x");
  Serial.println(lcd.getID(), HEX);
  
  Serial.print("FIRMWARE: 0x");
  Serial.println(lcd.getFirmwareVersion(), HEX);
  
  while(1);
}

