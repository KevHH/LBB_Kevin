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
const int buzzerPin = 10; // D10 pin connecting to buzzer
const int speakerLPin = 4; // D4 pin connecting to left speaker
const int speakerRPin = 7; // D7 pin connecting to right speaker
const int ledPin =  LED_BUILTIN;// the number of the LED pin
const int rMotLightPin = 8;
const int lMotLightPin = 9;

const int ldrLPin = A6;
const int ldrRPin = A7;

const int musicLedPin = 11; // D11 pin connecting to the pin for music

const int rMotEnaPin = 6; // D6 pin connecting to right motor
const int rMotEncBacPin = A1; // A1 pin connecting to the back encoder of the right motor
const int rMotEncFrtPin = A2; // A2 pin connecting to the front encoder of the right motor
const int rMotYBacPin = A0; // A0 pin connecting to the back Y pin of the right motor
const int rMotYFrtPin = A3; // A3 pin connecting to the front Y pin of the right motor

const int fanEnaPin = 5; // D5 pin connecting to fan 
const int fanYClockPin = A4; // A4 pin connecting to clockwise Y pin of fan
const int fanYAntiCPin = A5; // A5 pin connecting to anticlockwise Y pin of fan

// Bot constants
const int rMotPwmFreq = 255;
const long turnsPerRotation = 5148; 
const int minTimePerRotation = 12000; // in milliseconds, at full speed, ie pwm 255
 
const int fanPwmFreq = 100;
// Music related constants and variables
const float musicFreq[] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88};
const float musicSharpFreq[] = {277.18, 311.13, 0, 369.99, 415.3, 466.16, 0};

const int defaultBeatLength = minTimePerRotation / 96;
int baseBeatLength = defaultBeatLength;
int baseOctave = 2;
const char *jingleBellSheet[] = {"E", "E", "E l2", "E", "E", "E l2", "E", "G", "C l1.5", "D l0.5",
                                 "E l4",  "F", "F", "F l1.5", "F l0.5", "F", "E", "E", "E l0.5", "E l0.5", 
                                 "E", "D", "D", "E", "D l2", "G l2", "E", "E", "E l2", "E", "E", "E l2", 
                                 "E", "G", "C l1.5", "D l0.5", "E l4", "F", "F", "F", "F",
                                 "F", "E", "E", "E l0.5", "E l0.5", "G", "G", "F", "D", "C l4"};

const char *GoTSheet[] = {
                            "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", 
                            "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", 
                            "Gl3", "Cl3", "El2", "F", "Gl3", "Cl3", "El2", "F", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18", 
                            "Fl9", "A#o-l9", "D#l2", "D", "Fl6", "A#o-l9", "D#l2", "D", "Cl33", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl10", "l10", "Fl9", "A#o-l9", "D#l2", "D", "Fl6", "A#o-l9", "D#l2", "D", "Cl33", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18",
                            "Fl9", "A#o-l9", "Dl5", "D#l4", "Dl5", "A#o-l4", "Cl18", "l18",  
                            "Co+l18", "A#l18", "Cl18", "Gl18", "D#l18", "D#l18", "Fl18", "Gl18", 
                            "Co+l18", "A#l18", "Cl18", "Gl18", "D#l18", "D#l9", "Dl9", "Co+l3", "Gl3", "G#l2", "A#", "Co+l3", "Gl3", "G#l2", "A#", "Co+l3", "Gl3", 
                            "G#l2", "A#", "Co+l3", "Gl3", "G#l2", "A#l7", "G#o+l2", "A#o+", 
                            "Co+l3", "Gl3", "G#l2", "A#", "Co+l3", "Gl3", "G#l2", "A#", "Co+l3", "Gl3", "G#l2", "A#l5",
                            "Co++l3", "Go+l3", "G#o+l2", "A#o+", "Co++l3", "Go+l3", "G#o+l2", "A#o+ ", "Co++l3", "Go+l3", "G#o+l2", "A#o+l15"                        
                          };

long sheetIndex = 0;
bool nextTone = true;

bool rMotMoveFwd = true;
bool fanAntiCFwd = true;

bool musicOn = false;
bool LEDOn = false;
bool wheelOn = false;
bool fanOn = false;

// Buzzer
int buzzerState = LOW;


// Time control
unsigned long previousTime = 0;
long lag = 0;

byte botStage = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  
  pinMode(musicLedPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(speakerLPin, OUTPUT);
  pinMode(speakerRPin, OUTPUT);


  pinMode(fanEnaPin, OUTPUT);
  pinMode(fanYClockPin, OUTPUT);
  pinMode(fanYAntiCPin, OUTPUT);

  pinMode(rMotEnaPin, OUTPUT);
  pinMode(rMotYFrtPin, OUTPUT);
  pinMode(rMotYBacPin, OUTPUT);

  
  pinMode(rMotLightPin, OUTPUT);
  pinMode(lMotLightPin, OUTPUT);


  Serial.begin(9600);
}

void loop() {
  int ldrLValue = analogRead(ldrLPin); // do something when it's below 1000
  int ldrRValue = analogRead(ldrRPin);
  
  // switch for the bot
  int button = digitalRead(buttonPin);
  if (button == HIGH){
    // turn relevant variables off
    toggleBool(botOn);
    toggleState(ledState);
    digitalWrite(ledPin, ledState);

    botReset();
  }
  

  // things to be run when bot is on
  if (botOn) {
    unsigned long currentTime = millis();
    switch (botStage){
      case 0:
        if (ldrLValue < 1000 && ldrRValue < 1000){
          botStage = 1;
          musicOn = true;
          baseBeatLength = defaultBeatLength * 2;
        } else {
          if (currentTime - previousTime >= 1000){
            digitalWrite(buzzerPin, LOW);
            previousTime = millis();
          } else if (currentTime - previousTime >= 500){
            digitalWrite(buzzerPin, HIGH);
          }
        } 
        break;
      case 1:
        if (sheetIndex >= ARRAY_SIZE(jingleBellSheet)){
          botStage = 2;
          baseBeatLength = defaultBeatLength;
          baseOctave += 2;
          sheetIndex = 0;
          LEDOn = true;
          fanOn = true;
        } 
        break;
      case 2:        
        if (ldrLValue < 900){
            analogWrite(rMotEnaPin, rMotPwmFreq);
            digitalWrite(rMotYFrtPin, HIGH); 
            digitalWrite(rMotYBacPin, LOW);
            digitalWrite(rMotLightPin, HIGH);
            digitalWrite(lMotLightPin, HIGH);
            baseBeatLength = defaultBeatLength / 1.5;          
        }
        if (ldrRValue < 900){
            analogWrite(rMotEnaPin, rMotPwmFreq);
            digitalWrite(rMotYFrtPin, LOW); 
            digitalWrite(rMotYBacPin, HIGH);
            digitalWrite(rMotLightPin, HIGH);
            digitalWrite(lMotLightPin, HIGH);
          baseBeatLength = defaultBeatLength / 1.5;
          
        } 
        if (sheetIndex >= ARRAY_SIZE(jingleBellSheet)){
          botStage = 3; 
          baseBeatLength = defaultBeatLength / 2;
          wheelOn = true;
          fanOn = true;
          baseOctave += 1;
          sheetIndex = 0;
        } 
        break;
      case 3:
        if (sheetIndex >= ARRAY_SIZE(jingleBellSheet)){
          botStage = 4; 
          baseBeatLength = defaultBeatLength / 2;
          baseOctave = 3;
          sheetIndex = 0;
        } 

        break;
      case 4:
      
        // detect end of sheet
        if (sheetIndex >= ARRAY_SIZE(GoTSheet)){
          botOn = false;
          botReset();
          ledState = LOW;
          digitalWrite(ledPin, ledState);
          botStage = 0;
        } 
        break;
    }
    if (musicOn){
      // handles music
      if (currentTime - previousTime >= lag){
        if (nextTone){  
          // play next tone and set lag
          float beatUnits;
          if (botStage < 4){
            beatUnits = renderToneFromString(String(jingleBellSheet[sheetIndex]));
             
          } else {
            beatUnits = renderToneFromString(String(GoTSheet[sheetIndex]));
          }
          lag = round(beatUnits * baseBeatLength);
          
          // turn on LED
          if (LEDOn){
            digitalWrite(musicLedPin, HIGH);
          }

          // rotate wheel      
          if (wheelOn){
            digitalWrite(rMotLightPin, HIGH);
            digitalWrite(lMotLightPin, HIGH);
            analogWrite(rMotEnaPin, rMotPwmFreq);
            if (rMotMoveFwd){
              digitalWrite(rMotYFrtPin, HIGH); 
              digitalWrite(rMotYBacPin, LOW);
            } else {
              digitalWrite(rMotYFrtPin, LOW); 
              digitalWrite(rMotYBacPin, HIGH);
            }
            toggleBool(rMotMoveFwd);
          }

          // rotate fan    
          if (fanOn){
            analogWrite(fanEnaPin, fanPwmFreq);
            if (fanAntiCFwd){
              digitalWrite(fanYAntiCPin, HIGH); 
              digitalWrite(fanYClockPin, LOW);
            } else {
              digitalWrite(fanYAntiCPin, LOW); 
              digitalWrite(fanYClockPin, HIGH);
            }
            toggleBool(fanAntiCFwd);
          }
          // set up for next iteration
          sheetIndex += 1;

        } else {        
          botPause();
          if (botStage == 2){
            
        baseBeatLength = defaultBeatLength;
          }
        }

        toggleBool(nextTone);
        previousTime = currentTime;
      }
    }
  } else {
    botReset();          
  }
}

void botPause(){
  digitalWrite(musicLedPin, LOW);
    
  analogWrite(rMotEnaPin, 0);
  digitalWrite(rMotYFrtPin, LOW);
  digitalWrite(rMotYBacPin, LOW);

  analogWrite(fanEnaPin, 0);
  digitalWrite(fanYAntiCPin, LOW);
  digitalWrite(fanYClockPin, LOW);
  
            digitalWrite(rMotLightPin, LOW);
            digitalWrite(lMotLightPin, LOW);
    
}

void botReset(){
  botPause();
    
  rMotMoveFwd = true;
  lag = 0;
  nextTone = true;    
  
  // buffer time to decrease sensitivity of the button
  delay(200);
  previousTime = millis();
  sheetIndex = 0;
  botStage = 0;
  musicOn = false;
  fanOn = false;
  wheelOn = false;
  LEDOn = false;
}

float renderToneFromString(String toneString){
  int musicTone = 0;
  bool sharpOn = false;

  int octave = baseOctave;
  float beatUnits = 1;
  
  long i=0;
  long startIndex=0;
  while (i < toneString.length()){
    char currentChar = toneString[i];
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
        startIndex=i;
        while (i < toneString.length() && toneString[i] != ' '){
          i++;
        }
        beatUnits = toneString.substring(startIndex, i).toFloat();     
        break;
      case 'o':
        i++;
        startIndex = i;
        while (i < toneString.length() && toneString[i] != ' '){
          if (toneString[i] == '-'){
            octave -= 1;
          } else if (toneString[i] == '+'){
            octave += 1;
          }
          i++;
        }  
        break;
    }
    i++;
  }
  if (musicTone >=1 && musicTone <=7){
    playMusicTone(musicTone, sharpOn, octave, round(baseBeatLength * beatUnits));
  }
  return beatUnits;
}

void playMusicTone(int musicTone, bool sharpOn, int octave, long duration){
  int refFreq = 0;
  if (!sharpOn || musicSharpFreq[musicTone-1] == 0){
    refFreq = musicFreq[musicTone-1] * pow(2, octave - 4);
  } else {
    refFreq = musicSharpFreq[musicTone-1] * pow(2, octave - 4);
  }
  tone(speakerLPin, refFreq, duration);
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
