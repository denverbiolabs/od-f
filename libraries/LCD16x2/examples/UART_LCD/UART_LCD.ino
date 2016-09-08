#include <LCD16x2.h>
#include <Wire.h>

#define del  50

LCD16x2 lcd;
unsigned char buttons = 0xFF;
unsigned char blk  = 0;

void setup(){
  
  delay(1000);
  Wire.begin();
  Serial.begin(9600);
  
  lcd.uartEnable(true);
  
  // Clear the LCD
  Serial.println("CLEAR");
  delay(100);
  
  Serial.println("GOTOXY:2:1");
  delay(100);
  Serial.println("WR:BUT1>");
  delay(100);
  Serial.println("GOTOXY:10:1");
  delay(100);
  Serial.println("WR:BUT2>");
  delay(100);
  Serial.println("GOTOXY:2:2");
  delay(100);
  Serial.println("WR:BUT3>");
  delay(100);
  Serial.println("GOTOXY:10:2");
  delay(100);
  Serial.println("WR:BUT4>");
  delay(100);
}
void loop(){
  
  if(blk == 0)
    Serial.println("BLKL:0");
  else
    Serial.println("BLKL:255");
  blk ^= 0x01;
  
  Serial.println("BUT");
  while(Serial.available() > 0)
    buttons = Serial.read();
  
  delay(del);  
  Serial.println("GOTOXY:8:1");
  if(buttons & 0x01)
    Serial.println("WR:0");
  else
    Serial.println("WR:1");
    
  delay(del);  
  Serial.println("GOTOXY:15:1");
  if(buttons & 0x02)
    Serial.println("WR:0");
  else
    Serial.println("WR:1");
    
  delay(del);  
  Serial.println("GOTOXY:8:2");
  if(buttons & 0x04)
    Serial.println("WR:0");
  else
    Serial.println("WR:1");
    
  delay(del);  
  Serial.println("GOTOXY:15:2");
  if(buttons & 0x08)
    Serial.println("WR:0");
  else
    Serial.println("WR:1");
  
  delay(100);
}

