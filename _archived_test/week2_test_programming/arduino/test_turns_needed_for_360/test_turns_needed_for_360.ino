/*

  Test code by Kevin to test number of turns needed for one rotation


  255 - 187500
*/


int pwmFreq = 255;
const long guessTurns = 5140; // CONFIRMED 

bool botOn = false;


const int rMotEnaPin = 9; // A3 pin connecting to right motor
const int rMotEncBacPin = A1; // A1 pin connecting to the back encoder of the right motor
const int rMotEncFrtPin = A2; // A2 pin connecting to the front encoder of the right motor
const int rMotYBacPin = 10; // D11 pin connecting to the back Y pin of the right motor
const int rMotYFrtPin = 8; // D10 pin connecting to the front Y pin of the right motor
const int buttonPin = 12; // D12 pin connecting to the button

const int ledPin =  LED_BUILTIN;

byte rMotFrtDefValue = 0; // to store the default state
byte rMotBacDefValue = 0; // to store the default state

byte rMotState = 0; // 1 means forward, -1 means backward, 0 means still
long rMotTurns = 0;

void setup() {
  // put your setup code here, to run once:
  
  pinMode(rMotEnaPin, OUTPUT);
  pinMode(rMotYFrtPin, OUTPUT);
  pinMode(rMotYBacPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  // enable interrupt
  pciSetup(rMotEncBacPin);
  pciSetup(rMotEncFrtPin);  
}

void loop() {

  int button = digitalRead(buttonPin);
  if (button == HIGH){
    if (!botOn){
      botInit();
      botOn = true;
    } else {
      botDie();
      botOn = false;
    }
  }
          
  if (rMotTurns >= guessTurns){
    digitalWrite(ledPin, HIGH);
    botDie();
  }

}

void botInit(){
  // buffer time to decrease sensitivity
  delay(200);
  
  rMotTurns = 0;
  rMotState = 0;

  digitalWrite(ledPin, LOW);
  analogWrite(rMotEnaPin, pwmFreq);
  digitalWrite(rMotYFrtPin, HIGH);
  digitalWrite(rMotYBacPin, LOW);
  // record starting position
  rMotFrtDefValue = digitalRead(rMotEncFrtPin);
  rMotBacDefValue = digitalRead(rMotEncBacPin);
}

void botDie(){  
  analogWrite(rMotEnaPin, 0);
  digitalWrite(rMotYFrtPin, LOW);
  digitalWrite(rMotYBacPin, LOW);
  
  // buffer time to decrease sensitivity
  delay(200);
  
  rMotTurns = 0;
  rMotState = 0;
}

ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
  byte rMotFrtValue = digitalRead(rMotEncFrtPin);
  byte rMotBacValue = digitalRead(rMotEncBacPin);
  
  if (botOn){
    if (rMotBacValue == rMotBacDefValue && rMotFrtValue != rMotFrtDefValue){ // back activated first: moving forward
      rMotState = 1;
    } else if (rMotBacValue != rMotBacDefValue && rMotFrtValue == rMotFrtDefValue){ // front activated first: moving backward
      rMotState = -1;
    } else if (rMotBacValue == rMotBacDefValue && rMotFrtValue == rMotFrtDefValue){
      rMotTurns += rMotState;
    }
  }
}  

ISR (PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
      
} 
 
/**
* Helper function
*/


void toggleBool(bool &boolvar){
  if (boolvar){
    boolvar = false;
  } else {
    boolvar = true;
  }
}


/**
 * PIN CHANGE INTERRUPT ON ALL PINS https://playground.arduino.cc/Main/PinChangeInterrupt/
 */

 // Install Pin change interrupt for a pin, can be called multiple times
 
void pciSetup(byte pin)
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group
}
 
// Use one Routine to handle each group
 

ISR (PCINT2_vect) // handle pin change interrupt for D0 to D7 here
 {
 }  
 
