/*

  Test code by Kevin to play musical tones

*/

// helper functions
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

// overall control
bool botOn = false;
int ledState = LOW;

// Pin constants
const int buttonPin = 12; // D12 pin connecting to the button
const int buzzerPin = 2; // D2 pin connecting to buzzer
const int ledPin =  LED_BUILTIN;// the number of the LED pin

const int musicLedPin = 7; // D7 pin connecting to the pin for music

const int rMotEnaPin = 9; // A3 pin connecting to right motor
const int rMotEncBacPin = A1; // A1 pin connecting to the back encoder of the right motor
const int rMotEncFrtPin = A2; // A2 pin connecting to the front encoder of the right motor
const int rMotYBacPin = 10; // D11 pin connecting to the back Y pin of the right motor
const int rMotYFrtPin = 8; // D10 pin connecting to the front Y pin of the right motor

// Music related constants and variables
const float musicFreq[] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88};
const float musicSharpFreq[] = {277.18, 311.13, 0, 369.99, 415.3, 466.16, 0};

const int baseBeatLength = pow(2, 8);
const int baseOctave = 5;
const String jingleBellSheet[] = {"E", "E", "E l2", "E", "E", "E l2", "E", "G", "C l1.5", "D l0.5",
                                 "E l4",  "F", "F", "F l1.5", "F l0.5", "F", "E", "E", "E l0.5", "E l0.5", 
                                 "E", "D", "D", "E", "D l2", "G l2", "E", "E", "E l2", "E", "E", "E l2", 
                                 "E", "G", "C l1.5", "D l0.5", "E l4", "F", "F", "F", "F",
                                 "F", "E", "E", "E l0.5", "E l0.5", "G", "G", "F", "D", "C l4"};
int sheetIndex = 0;
bool nextTone = true;

bool rMotMoveFwd = true;

// Buzzer
int buzzerState = LOW;


// Time control
unsigned long previousTime = 0;
int lag = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  
  pinMode(musicLedPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  pinMode(rMotEnaPin, OUTPUT);
  pinMode(rMotYFrtPin, OUTPUT);
  pinMode(rMotYBacPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // switch for the bot
  int button = digitalRead(buttonPin);
  if (button == HIGH){
    // turn relevant variables off
    toggleBool(botOn);
    toggleState(ledState);
    digitalWrite(ledPin, ledState);
    // buffer time to decrease sensitivity of the button
    delay(200);
    
    // initialise
    previousTime = millis();
  }
  
  // detect end of sheet
  if (sheetIndex >= ARRAY_SIZE(jingleBellSheet)){
    botOn = false;
    ledState = LOW;
    digitalWrite(ledPin, ledState);
  } 

  // things to be run when bot is on
  if (botOn) {
    // detects pulses

    // handles music
    unsigned long currentTime = millis();
    if (currentTime - previousTime >= lag){
      
      if (nextTone){  
        // play next tone and set lag
        float beatUnits = playToneFromChar(jingleBellSheet[sheetIndex]);
        lag = round(beatUnits * baseBeatLength);
        
        // turn on LED
        digitalWrite(musicLedPin, HIGH);

        // rotate wheel       
        analogWrite(rMotEnaPin, 255);
        if (rMotMoveFwd){
          digitalWrite(rMotYFrtPin, HIGH); 
          digitalWrite(rMotYBacPin, LOW);
        } else {
          digitalWrite(rMotYFrtPin, LOW); 
          digitalWrite(rMotYBacPin, HIGH);
        }
        
        // set up for next iteration
        sheetIndex += 1;
        toggleBool(rMotMoveFwd);
      } else {        
        // turn LED off
        digitalWrite(musicLedPin, LOW);
        
        // stop wheel
        analogWrite(rMotEnaPin, 0);
        digitalWrite(rMotYFrtPin, LOW);
        digitalWrite(rMotYBacPin, LOW);           
      }

      toggleBool(nextTone);
      previousTime = currentTime;
    }
  } else {
    // reset
    digitalWrite(musicLedPin, LOW);
      
    analogWrite(rMotEnaPin, 0);
    digitalWrite(rMotYFrtPin, LOW);
    digitalWrite(rMotYBacPin, LOW);
    rMotMoveFwd = true;
    lag = 0;
    nextTone = true;    
    
    delay(200);
    previousTime = millis();
    sheetIndex = 0;
      
  }
}

float playToneFromChar(String toneChar){
  int musicTone = 0;
  bool sharpOn = false;
  int octave = baseOctave;
  float beatUnits = 1;
  
  int i=0;
  while (i < toneChar.length()){
    char currentChar = toneChar[i];
    switch (currentChar){
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
        musicTone = currentChar - 66;
        break;        
      case 'A':
      case 'B':
        musicTone = currentChar - 59;
        break;
      case '#':
        sharpOn = true;
        break;
      case 'l':
        i++;
        int startIndex=i;
        while (i < toneChar.length() && toneChar[i] != ' '){
          i++;
        }
        beatUnits = toneChar.substring(startIndex, i).toFloat();     
        break;
    }
    i++;
  }
  playMusicTone(musicTone, sharpOn, octave, round(baseBeatLength * beatUnits));
  return beatUnits;
}

void playMusicTone(int musicTone, bool sharpOn, int octave, int duration){
  int refFreq = 0;
  if (!sharpOn || musicSharpFreq[musicTone-1] == 0){
    refFreq = musicFreq[musicTone-1] * pow(2, octave - 4);
  } else {
    refFreq = musicFreq[musicTone-1] * pow(2, octave - 4);
  }

  tone(buzzerPin, refFreq, duration);
}

// ==================
// Helper functions
// ==================


void toggleBool(bool &boolvar){
  if (boolvar){
    boolvar = false;
  } else {
    boolvar = true;
  }
}

void toggleState(int &state){
  if (state == LOW){
    state = HIGH;
  } else {
    state = LOW;
  }
}
