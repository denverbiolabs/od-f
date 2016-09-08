#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd;

int buttons;

void setup(){
  Wire.begin();

  lcd.lcdClear();
  
  lcd.lcdGoToXY(2,1);
  lcd.lcdWrite("BUT1:");
  
  lcd.lcdGoToXY(10,1);
  lcd.lcdWrite("BUT2:");
  
  lcd.lcdGoToXY(2,2);
  lcd.lcdWrite("BUT3:");
  
  lcd.lcdGoToXY(10,2);
  lcd.lcdWrite("BUT4:");
}
void loop(){
  
  buttons = lcd.readButtons();
  
  lcd.lcdGoToXY(7,1);
  if(buttons & 0x01)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,1);
  if(buttons & 0x02)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(7,2);
  if(buttons & 0x04)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  lcd.lcdGoToXY(15,2);
  if(buttons & 0x08)  
    lcd.lcdWrite("0");
  else
    lcd.lcdWrite("1");
    
  delay(100);

}
