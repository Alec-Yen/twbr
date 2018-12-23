//Libraries 
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address, if it's not working try 0x27.
double MAX_VOLTAGE = 3.3; 
double R1 = 1000;
double R2 = 5100;
int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
double calibration = 12/11.785; // TODO: set as expected voltage/reported voltage

void setup(){
  // for lcd
  lcd.begin(16,2);   // iInit the LCD for 16 chars 2 lines
  lcd.backlight();   // Turn on the backligt (try lcd.noBaklight() to turn it off)

  // for battery
  pinMode(A0,INPUT);
  analogReadResolution(12);
  Serial.begin(9600);
}

void loop() {

  char buf1[17];
  char buf2[17];
  
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);
  double measuredVoltage = (double)sensorValue/4095*MAX_VOLTAGE*(R1+R2)/R1*calibration;
  Serial.print(sensorValue);
  Serial.print("   ");
  Serial.println(measuredVoltage);

  sprintf(buf1,"Battery Voltage:");
  sprintf(buf2,"%f V",measuredVoltage);
  lcd.clear();
  lcd.setCursor(0,0); //First line
  lcd.print(buf1);
  lcd.setCursor(0,1); //Second line  
  lcd.print(buf2);
  delay(1000);
}
