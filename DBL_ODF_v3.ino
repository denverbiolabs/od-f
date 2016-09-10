#include <stdint.h>
#include <LCD16x2.h>
#include <Wire.h>
#include <math.h>

LCD16x2 lcd;

const int SENSOR_OD = 2; 
const int SENSOR_F = 3;

const int LED_OD = 10;
const int LED_F = 11;

const int AVERAGE_WINDOW = 5;

const int BLANK = 0;
const int OD = 1;
const int F = 2;

// volatile to be stored as close to the µC as possible
volatile unsigned long sensor_count_f = 0;
unsigned long old_sensor_count_f = 0;

double blank_value;
double od_value;
double f_value;
double reading_blank;
double transmittance;
double percent_transmittance;
double absorbance;

int button_mode = 0;

volatile unsigned long sensor_count_od = 0;
unsigned long old_sensor_count_od = 0;

int buttons;

// The sensor translights light intensity to rising and falling frequency (a square wave). The interrupt function adds a count to sensor_count_od on every rising edge.
void interruptOD()
{
  sensor_count_od++;
}

// The sensor translights light intensity to rising and falling frequency (a square wave). The interrupt function adds a count to sensor_count_f on every rising edge.
void interruptF()
{
  sensor_count_f++;
}


unsigned long sensorRead(int mode){
  unsigned long reading = 0;
  if(mode == BLANK) {
    reading = blank();
  } else if (mode == OD) {
    reading = odRead();
  } else if (mode == F) {
    reading = fRead();
  }

  return reading;
}


unsigned long blank() {
  int count = 0;  
  int i = 0;
  unsigned long start = 0;
  unsigned long old_sensor_count_od = 0;
  unsigned long readings_window[AVERAGE_WINDOW];
  unsigned long current_reading_average;
  
  for (i = 0; i < AVERAGE_WINDOW; i++)
  {
    start = millis();
    old_sensor_count_od = sensor_count_od;

    // wait until at least 2 seconds have passed
    while(millis()-start <= 1000) { } 
  
    unsigned long current_sensor_count = sensor_count_od;
    unsigned long current_reading = 0;
    current_reading = current_sensor_count - old_sensor_count_od;
    
    Serial.print("Blank: ");
    Serial.println(current_reading);
    

    // add the current sensor reading to the array, to be averaged later
    readings_window[count++ % AVERAGE_WINDOW] = current_reading;

    // reset global variables to zero
    sensor_count_od = 0;
  }

  // take the average of the five most current sensor readings stored in the array
  unsigned long sum_of_readings = 0;
  for (i = 0; i < AVERAGE_WINDOW; i++)
  {
    sum_of_readings += readings_window[i];
  }
  Serial.print("Sum: ");
  Serial.println(sum_of_readings);

  current_reading_average = sum_of_readings / AVERAGE_WINDOW;
  Serial.print("Blank average: ");
  Serial.println(current_reading_average);

  blank_value = current_reading_average;
  return current_reading_average;
}

unsigned long odRead() {
  int count = 0;  
  int i = 0;
  unsigned long start = 0;
  unsigned long old_sensor_count_od = 0;
  unsigned long readings_window[AVERAGE_WINDOW];
  unsigned long current_reading_average = 0;
  
  for (i = 0; i < AVERAGE_WINDOW; ++i)
  {
    start = millis();
    old_sensor_count_od = sensor_count_od;

    // wait until at least 1 seconds have passed
    while(millis()-start <= 1000) { } 
  
    unsigned long current_sensor_count = sensor_count_od;
    unsigned long current_reading = 0;
    current_reading = current_sensor_count - old_sensor_count_od;
    
    Serial.print("OD Reading: ");
    Serial.println(current_reading);
    

    // add the current sensor reading to the array, to be averaged later
    readings_window[count++ % AVERAGE_WINDOW] = current_reading;

    // reset global variables to zero
    sensor_count_od = 0;
  }

  // take the average of the five most current sensor readings stored in the array
  unsigned long sum_of_readings = 0;
  for (i = 0; i < AVERAGE_WINDOW; ++i)
  {
    sum_of_readings += readings_window[i];
  }
  
  current_reading_average = sum_of_readings / AVERAGE_WINDOW;
  Serial.print("OD Reading: ");
  Serial.println(current_reading_average);
  
  od_value = current_reading_average;
  return current_reading_average;
}

unsigned long fRead() {
  int count = 0;
  int i = 0;
  unsigned long start = 0;
  unsigned long old_sensor_count_f = 0;
  unsigned long readings_window[AVERAGE_WINDOW];
  unsigned long current_reading_average = 0;
  
  for (i = 0; i < AVERAGE_WINDOW; ++i)
  {
    start = millis();
    old_sensor_count_f = sensor_count_f;

    // wait until at least 4 seconds have passed
    while(millis()-start <= 4000) { } 
  
    unsigned long current_sensor_count = sensor_count_f;
    unsigned long current_reading = 0;
    current_reading = current_sensor_count - old_sensor_count_f;
    
    Serial.print("Fluorescent Reading: ");
    Serial.println(current_reading);
   

    // add the current sensor reading to the array, to be averaged later
    readings_window[count++ % AVERAGE_WINDOW] = current_reading;

    // reset global variables to zero
    sensor_count_f = 0;
  }

  // take the average of the five most current sensor readings stored in the array
  unsigned long sum_of_readings = 0;
  for (i = 0; i < AVERAGE_WINDOW; ++i)
  {
    sum_of_readings += readings_window[i];
  }
  
  current_reading_average = sum_of_readings / AVERAGE_WINDOW;
  Serial.print("Fluorescent Average Reading: ");
  Serial.println(current_reading_average);
  
  f_value = current_reading_average;
  return current_reading_average;
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  pinMode(SENSOR_OD, INPUT);
  digitalWrite(SENSOR_OD, HIGH);
  attachInterrupt(digitalPinToInterrupt(2), interruptOD, RISING); //interrupt is on PIN 2
  
  pinMode(SENSOR_F, INPUT);
  digitalWrite(SENSOR_F, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), interruptF, RISING); //interrupt is on PIN 3

  pinMode(LED_OD, OUTPUT);
  digitalWrite(LED_OD, HIGH);

  pinMode(LED_F, OUTPUT);
  digitalWrite(LED_F, HIGH);
  
  lcd.lcdSetBlacklight(100);
  lcd.lcdClear();
  lcd.lcdGoToXY(2, 1);
  lcd.lcdWrite("Denver Biolabs");
  lcd.lcdGoToXY(3, 2);
  lcd.lcdWrite("OD & F Meter");
}

void loop() {
 
  unsigned long hz; //current frequency
  unsigned long wait = 0;
 
  buttons = lcd.readButtons();

  if (!(buttons & 0x01)) {
    lcd.lcdClear();
    lcd.lcdGoToXY(1, 1);
    lcd.lcdWrite("Reading OD Blank");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");
    
    reading_blank = sensorRead(BLANK);
    
    Serial.print("BLANK READING in loop: " );
    Serial.println(reading_blank);
    
    lcd.lcdClear();
    lcd.lcdGoToXY(1, 1);
    lcd.lcdWrite("Load Sample then");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("press 'Read OD'");
    
  }

  if (!(buttons & 0x02)) {
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("Reading Sample");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");
    
    unsigned long reading_od = sensorRead(OD);
    
    Serial.print("OD READING in loop: ");
    Serial.println(reading_od);
   
    transmittance = reading_od / reading_blank;
    percent_transmittance = transmittance * 100;
    
    Serial.print("Transmittance: ");
    Serial.println(transmittance, 5);
    Serial.print("Transmittance %: ");
    Serial.println(percent_transmittance, 5);

    absorbance = 2 - (log10(percent_transmittance));
    
    Serial.print("Absorbance: ");
    Serial.println(absorbance, 5);
    
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("OD600: ");
    lcd.lcdGoToXY(7, 1);
    lcd.lcdWrite(absorbance, 5);
    lcd.lcdGoToXY(1, 2);
    lcd.lcdWrite("T%: ");
    lcd.lcdGoToXY(5, 2);
    lcd.lcdWrite(percent_transmittance, 5);
  }

  if (!(buttons & 0x04)) {
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("Reading Sample");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");

    unsigned long reading_f = sensorRead(F);
    
    Serial.print("F READING in loop: ");
    Serial.println(reading_f);
  
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("AU: ");
    lcd.lcdGoToXY(5, 1);
    lcd.lcdWrite(reading_f, 5);
  }

  if (!(buttons & 0x08)) { 
    if (button_mode == 1) {
      digitalWrite(LED_OD, LOW);
      digitalWrite(LED_F, LOW);
      delay(500);
      button_mode = 0;
    } else {
      digitalWrite(LED_OD, HIGH);
      digitalWrite(LED_F, HIGH);
      delay(500);
      button_mode = 1;
    }
  }
  
 }

