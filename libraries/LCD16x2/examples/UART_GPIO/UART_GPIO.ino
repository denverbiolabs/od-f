#include <LCD16x2.h>
#include <Wire.h>

LCD16x2 lcd;
char gpio;

void setup(){
  delay(1000);
  Wire.begin();
  Serial.begin(9600);
  lcd.uartEnable(true);
  
  // GPIO1 as output
  Serial.println("TRIS:1:0");
  
  // GPIO6 as input
  Serial.println("TRIS:6:1");
  
  // Set Pullpin
  Serial.println("LAT:6:1");
}
void loop(){
  
  // Read GPIO
  Serial.println("PORT:6");
  while(Serial.available() > 0){
    gpio = Serial.read();
  }
  
  // Toggle GPIO1 according to GPIO6 state  
  if(gpio == '1')
    Serial.println("LAT:1:1");
  else
    Serial.println("LAT:1:0");
    
  delay(100);
  
}

