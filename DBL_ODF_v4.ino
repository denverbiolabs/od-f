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
const int OD_WAIT = 1000;
const int F_WAIT = 4000;

// volatile to be stored as close to the µC as possible
volatile unsigned long sensor_count_od = 0;
volatile unsigned long sensor_count_f = 0;

double blank_value;
double od_value;
double f_value;
double reading_blank;
double transmittance;
double percent_transmittance;
double absorbance;

int button_mode = 0;

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


// Takes a mode parameter (BLANK, OD, or F)
// Returns the sensor reading for the appropriate sensor
unsigned long sensorRead(int mode, unsigned long wait_time){
  int count = 0;  
  int i = 0;
  unsigned long start = 0;
  unsigned long old_sensor_count = 0;
  unsigned long readings_window[AVERAGE_WINDOW];
  unsigned long current_reading_average;
  
  // Variables that depend on which mode is selected (i.e., blank, OD, or F)
  unsigned long sensor_count;
  
  // For printing and debugging
  String mode_string;
  if(mode == BLANK){
    mode_string = "BLANK";
  } else if (mode == OD) {
    mode_string = "OD";
  } else if (mode == F) {
    mode_string = "F";
  }  

  for (i = 0; i < AVERAGE_WINDOW; i++)
  { 
    Serial.print(sensor_count_od);
    start = millis();
    
    if(mode == BLANK || mode == OD) {
       sensor_count = sensor_count_od; 
    } else if (mode == F) {
       sensor_count = sensor_count_f;
    }
    
    old_sensor_count = sensor_count;

    // wait for the specified time determined by the current mode
    while(millis()-start <= wait_time) { } 
  
    unsigned long current_sensor_count = sensor_count;
    unsigned long current_reading = 0;
    current_reading = current_sensor_count - old_sensor_count;
    
    Serial.print("Current Reading for " + mode_string + ": ");
    Serial.println(current_reading);
    
    // add the current sensor reading to the array, to be averaged later
    readings_window[count++ % AVERAGE_WINDOW] = current_reading;
    
    // reset global variables to zero
    if(mode == BLANK || mode == OD) {
      sensor_count_od = 0; 
    } else if (mode == F) {
      sensor_count_f = 0;
    }
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
  Serial.print("Average Reading: ");
  Serial.println(current_reading_average);

  if(mode == BLANK){
    blank_value = current_reading_average;
  } else if (mode == OD) {
    od_value = current_reading_average;
  } else if (mode == F) {
    f_value = current_reading_average;
  }
  
  return current_reading_average;

}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  pinMode(SENSOR_OD, INPUT);
  digitalWrite(SENSOR_OD, HIGH);
  attachInterrupt(digitalPinToInterrupt(SENSOR_OD), interruptOD, RISING); //interrupt is on PIN 2
  
  pinMode(SENSOR_F, INPUT);
  digitalWrite(SENSOR_F, HIGH);
  attachInterrupt(digitalPinToInterrupt(SENSOR_F), interruptF, RISING); //interrupt is on PIN 3

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
    // Start the interrupt routine for the OD sensor
    //attachInterrupt(digitalPinToInterrupt(SENSOR_OD), interruptOD, RISING);
    
    lcd.lcdClear();
    lcd.lcdGoToXY(1, 1);
    lcd.lcdWrite("Reading OD Blank");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");
    
    reading_blank = sensorRead(BLANK, OD_WAIT);
    
    Serial.print("BLANK READING in loop: " );
    Serial.println(reading_blank);
    
    lcd.lcdClear();
    lcd.lcdGoToXY(1, 1);
    lcd.lcdWrite("Load Sample then");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("press 'Read OD'");
    
    //detachInterrupt(digitalPinToInterrupt(SENSOR_OD));
  }

  if (!(buttons & 0x02)) {
    // Start the interrupt routine for the OD sensor
    //attachInterrupt(SENSOR_OD, interruptOD, RISING);
    
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("Reading Sample");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");
    
    unsigned long reading_od = sensorRead(OD, OD_WAIT);
    
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
    
    //detachInterrupt(SENSOR_OD);
  }

  if (!(buttons & 0x04)) {
    // Start the interrupt routine for the F sensor
    //attachInterrupt(SENSOR_F, interruptF, RISING);
    
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("Reading Sample");
    lcd.lcdGoToXY(2, 2);
    lcd.lcdWrite("Please Wait...");

    unsigned long reading_f = sensorRead(F, F_WAIT);
    
    Serial.print("F READING in loop: ");
    Serial.println(reading_f);
  
    lcd.lcdClear();
    lcd.lcdGoToXY(2, 1);
    lcd.lcdWrite("AU: ");
    lcd.lcdGoToXY(5, 1);
    lcd.lcdWrite(reading_f, 5);
    
    //detachInterrupt(SENSOR_F);
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

