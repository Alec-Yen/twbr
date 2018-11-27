/*

This example shows how to get start with Cytron 4 Channel Motor Driver (FD04A)
and Maker UNO (Arduino Uno compatible)

Product page:
  Cytron 4 Channel Motor Driver: https://www.cytron.io/p-fd04a
  Maker UNO: https://www.cytron.io/p-maker-uno

Created by:
  25/06/18  Idris, Cytron Technologies

 */

#define M1_DIR  4 // Motor direction channel 1 connected to pin 4
#define M1_EN   3 // Motor enable channel 1 connected to pin 3
#define M2_DIR  7// Motor direction channel 2 connected to pin 5
#define M2_EN   6 // Motor enable channel 2 connected to pin 9

#define NOTE_C4  262

void setup()
{
  Serial.begin(9600);
  pinMode(M1_DIR, OUTPUT); // Set Motor direction channel 1 as output
  pinMode(M1_EN, OUTPUT); // Set Motor enable channel 1 as output
  pinMode(M2_DIR, OUTPUT); // Set Motor direction channel 2 as output
  pinMode(M2_EN, OUTPUT); // Set Motor enable channel 2 as output
}

void loop()
{
  // If push button is pressed...
  if (Serial.read()) {

//    tone(BUZZER, NOTE_C4, 100);
  //  delay(1000);

    // Motor 1
    digitalWrite(M1_DIR, LOW);
    analogWrite(M1_EN, 80); // Low speed
    delay(2000);
    analogWrite(M1_EN, 255); // High speed
    delay(2000);
    analogWrite(M1_EN, 0); // Stop
    delay(1000);
    
    digitalWrite(M1_DIR, HIGH);
    analogWrite(M1_EN, 80); // Low speed
    delay(2000);
    analogWrite(M1_EN, 255); // High speed
    delay(2000);
    analogWrite(M1_EN, 0); // Stop
    delay(1000);

    // Motor 2
    digitalWrite(M2_DIR, LOW);
    analogWrite(M2_EN, 80); // Low speed
    delay(2000);
    analogWrite(M2_EN, 255); // High speed
    delay(2000);
    analogWrite(M2_EN, 0); // Stop
    delay(1000);
    
    digitalWrite(M2_DIR, HIGH);
    analogWrite(M2_EN, 80); // Low speed
    delay(2000);
    analogWrite(M2_EN, 255); // High speed
    delay(2000);
    analogWrite(M2_EN, 0); // Stop
    delay(1000);
  }
}
