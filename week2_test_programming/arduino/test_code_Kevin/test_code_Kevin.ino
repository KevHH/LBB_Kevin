/*

  Test code by Kevin to flash LEDs and ring the buzzer

 */
 
 // Pin constants
 const int buzzerPin = 9; // Analog output pin
 const int ledPin =  LED_BUILTIN;// the number of the LED pin

 // Time variables
 unsigned long previousMillis = 0;        // will store last time LED was updated
 const long interval = 1000;           // interval at which to blink (milliseconds)
 

 // State Variables
 int ledState = LOW;             // ledState used to set the LED
 int buzzerState = LOW;          // buzzerState used to set the buzzer

 void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
 }
 
 void loop() {
   // read the analog in value
   unsigned long currentMillis = millis();
   
   if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    if (buzzerState == LOW) {
      buzzerState = HIGH;
    } else {
      buzzerState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPin, ledState);
    digitalWrite(buzzerPin, buzzerState);
   }
 }
