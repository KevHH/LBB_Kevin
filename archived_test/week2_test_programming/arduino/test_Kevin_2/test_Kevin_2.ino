/*

  Test code by Kevin to flash LED and ring buzzer based on LDR input

 */
 
 // Pin constants
 const int buzzerPin = 9; // D9 pin connecting to buzzer
 const int ledPin =  LED_BUILTIN;// the number of the LED pin
 const int ldrPin = A7; // A7 pin connecting to LDR

 // Time variables
 unsigned long previousMillis = 0;        // will store last time LED was updated
 const long lag = 800;           // interval at which to blink (milliseconds) 

 // State Variables
 int buzzerState = LOW;          // buzzerState used to set the buzzer
 int previousldrValue = 0;

 void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    previousldrValue = analogRead(ldrPin);
    Serial.begin(9600);
 }
 
 void loop() {
   // read time
   unsigned long currentMillis = millis();
   // read ldr
   int currentldrValue = analogRead(ldrPin);

   // perform action only when lag is over
   if (currentMillis - previousMillis >= lag) {
    // restart the timer
    previousMillis = currentMillis;
    // log
    Serial.println(currentldrValue);
    Serial.println(previousldrValue);
    Serial.println("\n");

    // increasing
    if (currentldrValue > 130 || currentldrValue - previousldrValue >= 10) {
      // turn on LED
      digitalWrite(ledPin, HIGH);
      
    Serial.println("executed");
      // click the buzzer
      if (buzzerState == LOW) {
        buzzerState = HIGH;
      } else {
        buzzerState = LOW;
      }    
      digitalWrite(buzzerPin, buzzerState);
    } else {
      // turn off LED
      digitalWrite(ledPin, LOW);
    }
    
      // record for comparison
      previousldrValue = currentldrValue;
   }
 }
