#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd;

int intVal1 = 0, intVal2 = 0;
float floatVal1 = 0.0, floatVal2 = 0.0;


void setup(){
  Wire.begin();

  lcd.lcdClear();
  
  lcd.lcdGoToXY(1,1);
  lcd.lcdWrite("Int:");
  
  lcd.lcdGoToXY(1,2);
  lcd.lcdWrite("Float:");
}

void loop(){
  
  lcd.lcdGoToXY(8,1);
  lcd.lcdWrite(intVal1++);
  
  lcd.lcdGoToXY(13,1);
  lcd.lcdWrite(intVal2--);
    
  lcd.lcdGoToXY(8,2);
  lcd.lcdWrite(floatVal1, 1);
  floatVal1=floatVal1+0.1;
  
  lcd.lcdGoToXY(13,2);
  lcd.lcdWrite(floatVal2, 1);
  floatVal2=floatVal2-0.1;
    
  delay(500);
}
