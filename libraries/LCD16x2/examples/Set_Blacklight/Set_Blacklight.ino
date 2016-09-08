#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd;
const int analog_pin = A0;

void setup(){
  Wire.begin();
  pinMode(analog_pin, INPUT);
}
void loop(){
  
  int value = analogRead(analog_pin);
  map(value, 0, 1023, 0, 255);
  lcd.lcdSetBlacklight(value);
  delay(100);  
  
}

