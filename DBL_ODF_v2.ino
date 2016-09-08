#include <stdint.h>
#include <LCD16x2.h>
#include <Wire.h>

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
//unsigned long temp_f = 0;
//unsigned long last_f;

unsigned long blank_value;
unsigned long od_value;
unsigned long f_value;

int button_mode = 0;

volatile unsigned long sensor_count_od = 0;
unsigned long old_sensor_count_od = 0;
//unsigned long tOD = 0;
//unsigned long last_od;

// this is the recorded frequency. The average window is used to get more stable results
//unsigned long hz_refF; // blank frequency
//unsigned long hz_ref_avgF;
//unsigned long current_reading_average; // blank frequency
//unsigned long hz_ref_avgOD;

//unsigned long hz_avgF[AVERAGE_WINDOW];
//unsigned long hz_windowF = 0;
//unsigned long iCountF = 0;
//unsigned long readings_window[AVERAGE_WINDOW];
//unsigned long iCountOD = 0;
//unsigned long hz_windowOD = 0;

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
    Serial.print(current_reading);
    Serial.print("\n");

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
  Serial.print(sum_of_readings);
  Serial.print("\n");
  current_reading_average = sum_of_readings / AVERAGE_WINDOW;
  Serial.print("Blank average: ");
  Serial.print(current_reading_average);
  Serial.print("\n");
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

    // wait until at least 2 seconds have passed
    while(millis()-start <= 2000) { } 
  
    unsigned long current_sensor_count = sensor_count_od;
    unsigned long current_reading = 0;
    current_reading = current_sensor_count - old_sensor_count_od;
    
    Serial.print("OD Reading: ");
    Serial.print(current_reading);
    Serial.print("\n");

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
  Serial.print(current_reading_average);
  Serial.print("\n");
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
    Serial.print(current_reading);
    Serial.print("\n");

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
  Serial.print("Fluorescent Reading: ");
  Serial.print(current_reading_average);
  Serial.print("\n");
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
  
  //blank();
  
  lcd.lcdSetBlacklight(100);
  lcd.lcdClear();
  lcd.lcdGoToXY(2, 1);
  lcd.lcdWrite("Denver Biolabs");
  lcd.lcdGoToXY(3, 2);
  lcd.lcdWrite("OD & F Meter");
}

/*unsigned long getMeasurement(volatile unsigned long count, unsigned long old_count, unsigned long reading_array[], unsigned long current_reading, unsigned long current_reading_average, unsigned long temp_counter, unsigned long last_time, int wait_time)
{
  
  // Allow the specified amount of time ("wait_time") to pass before capturing another sensor count reading
  while (millis() - last_time <= wait_time) { }
  
  unsigned long temp = count;
  current_reading = temp - old_count;

 
  Serial.print("Blank TEST: ");
  Serial.print(current_reading);
  Serial.print("\n");

  reading_array[temp_counter % AVERAGE_WINDOW] = current_reading;
  temp_counter++;

  unsigned long sum_of_readings = 0;
  for (uint8_t i = 0; i < AVERAGE_WINDOW; ++i)
  {
    sum_of_readings += reading_array[i];
  }
  
  current_reading_average = sum_of_readings / AVERAGE_WINDOW; // windows average frequency
  old_count = temp;
  Serial.print("Average: ");
  Serial.print(current_reading_average + "\n");
  
  last_time = millis();
  count = 0;
  old_count = 0;
  
  return current_reading_average;
}*/

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
    unsigned long reading_blank = sensorRead(BLANK);
    Serial.print("BLANK READING in loop: " );
    Serial.print(reading_blank);

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
    Serial.print(reading_od);
    Serial.print("\n");
  
    //unsigned long hzF, hzOD; //current frequency
  
    //hzF = getMeasurement(sensor_count_f, old_sensor_count_f, hz_avgF,hzF, hz_windowF, iCountF, last_f, 4000);
    //hzOD = getMeasurement(sensor_count_od, old_sensor_count_od, readings_window,hzOD, hz_windowOD, iCountOD, last_od, 1000);
      
//    analogWrite(LED_OD, 255);
//    analogWrite(LED_F, 255);
//    delay(500);
//    
//    Serial.print("#;");
//    Serial.print(0);
//    Serial.print(";");
//    Serial.print(0);
//    Serial.print(";");
//    Serial.print(0);
//    Serial.print(";");
//    //Serial.print(current_reading_average);
//    Serial.print(";");
//    Serial.print(hzOD);
//    Serial.print(";");
//    Serial.print(hzF);
//    Serial.print("\n");
    
//    lcd.lcdClear();
//    lcd.lcdGoToXY(3, 1);
//    lcd.lcdWrite("OD: ");
//    lcd.lcdGoToXY(8, 1);
//    lcd.lcdWrite(hz_windowOD);
   
  }

  if (!(buttons & 0x04)) {
    //digitalWrite(LED_OD, HIGH);
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("Reading Sample");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");

    unsigned long reading_f = sensorRead(F);
    Serial.print("F READING in loop: ");
    Serial.print(reading_f);
    Serial.print("\n");
    
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

